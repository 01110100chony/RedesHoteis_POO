#include "TelaListagemReservas.h"
#include "TelaResumoReserva.h"
#include "core/GerenciadorReservas.h"
#include "model/TiposHotel.h"

#include <QAbstractItemView>
#include <QColor>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLocale>
#include <QMessageBox>
#include <QVBoxLayout>

TelaListagemReservas::TelaListagemReservas(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("Listagem de Reservas - Hotéis Paradise");
    setMinimumSize(1020, 540);

    auto* layoutPrincipal = new QVBoxLayout(this);
    layoutPrincipal->setContentsMargins(18, 18, 18, 18);
    layoutPrincipal->setSpacing(12);

    auto* lblTitulo = new QLabel("Reservas Registradas (Ordenadas por Check-in)", this);
    lblTitulo->setObjectName("tituloPrincipal");
    layoutPrincipal->addWidget(lblTitulo);

    // Barra de filtros e consulta de clientes
    auto* filtroLayout = new QHBoxLayout();
    filtroLayout->setSpacing(10);

    cmbFiltroUnidade = new QComboBox(this);
    cmbFiltroUnidade->addItem("Todas as unidades", -1);
    for (Unidade u : InfoHotel::todasUnidades()) {
        cmbFiltroUnidade->addItem(InfoHotel::unidadeParaTexto(u), static_cast<int>(u));
    }

    cmbFiltroStatus = new QComboBox(this);
    cmbFiltroStatus->addItem("Todos os status", -1);
    cmbFiltroStatus->addItem("Confirmada", static_cast<int>(StatusReserva::Confirmada));
    cmbFiltroStatus->addItem("Pendente de confirmação", static_cast<int>(StatusReserva::Pendente));
    cmbFiltroStatus->addItem("Cancelada", static_cast<int>(StatusReserva::Cancelada));

    auto* btnVerClientes = new QPushButton("Ver Clientes Cadastrados", this);

    filtroLayout->addWidget(new QLabel("Unidade:", this));
    filtroLayout->addWidget(cmbFiltroUnidade);
    filtroLayout->addWidget(new QLabel("Status:", this));
    filtroLayout->addWidget(cmbFiltroStatus);
    filtroLayout->addStretch();
    filtroLayout->addWidget(btnVerClientes);
    layoutPrincipal->addLayout(filtroLayout);

    // Tabela principal
    tabelaReservas = new QTableWidget(this);
    tabelaReservas->setColumnCount(12);
    tabelaReservas->setHorizontalHeaderLabels({
        "ID", "Check-in", "Check-out", "Diárias", "Unidade", "Quarto",
        "Cliente", "Atendente", "Desconto", "Total", "Entrada (1/3)", "Status"
    });
    tabelaReservas->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tabelaReservas->setSelectionBehavior(QAbstractItemView::SelectRows);
    tabelaReservas->setSelectionMode(QAbstractItemView::SingleSelection);
    tabelaReservas->setAlternatingRowColors(true);
    tabelaReservas->verticalHeader()->setVisible(false);
    tabelaReservas->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    tabelaReservas->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Stretch);
    tabelaReservas->horizontalHeader()->setSectionResizeMode(11, QHeaderView::ResizeToContents);

    layoutPrincipal->addWidget(tabelaReservas);

    lblTotalExibido = new QLabel(this);
    lblTotalExibido->setObjectName("subtitulo");
    layoutPrincipal->addWidget(lblTotalExibido);

    // Botoes de acao
    auto* acoesLayout = new QHBoxLayout();
    acoesLayout->setSpacing(10);

    btnConfirmarPagamento = new QPushButton("Confirmar Pagamento", this);
    btnConfirmarPagamento->setObjectName("btnSucesso");

    btnCancelarReserva = new QPushButton("Cancelar Reserva", this);
    btnVerResumo = new QPushButton("Ver Resumo", this);
    auto* btnExportar = new QPushButton("Exportar (.csv / .txt)", this);
    btnExportar->setObjectName("btnPrimario");
    auto* btnFechar = new QPushButton("Fechar", this);

    acoesLayout->addWidget(btnConfirmarPagamento);
    acoesLayout->addWidget(btnCancelarReserva);
    acoesLayout->addWidget(btnVerResumo);
    acoesLayout->addStretch();
    acoesLayout->addWidget(btnExportar);
    acoesLayout->addWidget(btnFechar);
    layoutPrincipal->addLayout(acoesLayout);

    connect(cmbFiltroUnidade, &QComboBox::currentIndexChanged, this, &TelaListagemReservas::aoAlterarFiltros);
    connect(cmbFiltroStatus, &QComboBox::currentIndexChanged, this, &TelaListagemReservas::aoAlterarFiltros);
    connect(tabelaReservas, &QTableWidget::itemSelectionChanged, this, &TelaListagemReservas::aoMudarSelecao);
    connect(tabelaReservas, &QTableWidget::cellDoubleClicked, this, [this](int, int) { aoVerResumo(); });

    connect(btnConfirmarPagamento, &QPushButton::clicked, this, &TelaListagemReservas::aoConfirmarPagamento);
    connect(btnCancelarReserva, &QPushButton::clicked, this, &TelaListagemReservas::aoCancelarReserva);
    connect(btnVerResumo, &QPushButton::clicked, this, &TelaListagemReservas::aoVerResumo);
    connect(btnVerClientes, &QPushButton::clicked, this, &TelaListagemReservas::aoVerClientes);
    connect(btnExportar, &QPushButton::clicked, this, &TelaListagemReservas::aoExportarArquivo);
    connect(btnFechar, &QPushButton::clicked, this, &QDialog::accept);

    carregarTabela();
}

int TelaListagemReservas::idReservaSelecionada() const
{
    int linha = tabelaReservas->currentRow();
    if (linha < 0) {
        return -1;
    }
    QTableWidgetItem* itemId = tabelaReservas->item(linha, 0);
    return itemId ? itemId->text().toInt() : -1;
}

void TelaListagemReservas::carregarTabela()
{
    tabelaReservas->setRowCount(0);

    int filtroUnidade = cmbFiltroUnidade->currentData().toInt();
    int filtroStatus = cmbFiltroStatus->currentData().toInt();

    const QList<Reserva> todas = GerenciadorReservas::getInstance().listarReservasOrdenadas();
    QLocale br(QLocale::Portuguese, QLocale::Brazil);

    int exibidas = 0;
    for (const Reserva& r : todas) {
        if (filtroUnidade != -1 && static_cast<int>(r.getUnidade()) != filtroUnidade) {
            continue;
        }
        if (filtroStatus != -1 && static_cast<int>(r.getStatus()) != filtroStatus) {
            continue;
        }

        int row = tabelaReservas->rowCount();
        tabelaReservas->insertRow(row);

        auto criarItem = [&](const QString& texto, Qt::Alignment alinhamento = Qt::AlignLeft | Qt::AlignVCenter) {
            auto* item = new QTableWidgetItem(texto);
            item->setTextAlignment(alinhamento);
            return item;
        };

        tabelaReservas->setItem(row, 0, criarItem(QString::number(r.getId()), Qt::AlignCenter));
        tabelaReservas->setItem(row, 1, criarItem(r.getDataCheckIn().toString("dd/MM/yyyy"), Qt::AlignCenter));
        tabelaReservas->setItem(row, 2, criarItem(r.getDataCheckOut().toString("dd/MM/yyyy"), Qt::AlignCenter));
        tabelaReservas->setItem(row, 3, criarItem(QString::number(r.getNumDiarias()), Qt::AlignCenter));
        tabelaReservas->setItem(row, 4, criarItem(InfoHotel::unidadeParaTexto(r.getUnidade())));
        tabelaReservas->setItem(row, 5, criarItem(InfoHotel::tipoQuartoParaTexto(r.getTipoQuarto())));
        tabelaReservas->setItem(row, 6, criarItem(r.getCliente().getResumoIdentificacao()));
        tabelaReservas->setItem(row, 7, criarItem(r.getNomeAtendente()));
        tabelaReservas->setItem(row, 8, criarItem(QString("%1 (%2%)")
                                                      .arg(r.getNomePoliticaDesconto())
                                                      .arg(r.getPercentualDesconto(), 0, 'f', 0)));
        tabelaReservas->setItem(row, 9, criarItem(br.toCurrencyString(r.getValorTotal(), "R$ "), Qt::AlignRight | Qt::AlignVCenter));
        tabelaReservas->setItem(row, 10, criarItem(br.toCurrencyString(r.getValorEntrada(), "R$ "), Qt::AlignRight | Qt::AlignVCenter));

        auto* itemStatus = criarItem(InfoHotel::statusParaTexto(r.getStatus()), Qt::AlignCenter);
        itemStatus->setData(Qt::UserRole, static_cast<int>(r.getStatus()));
        QFont fonteStatus = itemStatus->font();
        fonteStatus.setBold(true);
        itemStatus->setFont(fonteStatus);

        if (r.getStatus() == StatusReserva::Confirmada) {
            itemStatus->setBackground(QColor("#e8f5e9"));
            itemStatus->setForeground(QColor("#1b5e20"));
        } else if (r.getStatus() == StatusReserva::Pendente) {
            itemStatus->setBackground(QColor("#fff3e0"));
            itemStatus->setForeground(QColor("#e65100"));
        } else {
            itemStatus->setBackground(QColor("#ffebee"));
            itemStatus->setForeground(QColor("#b71c1c"));
        }
        tabelaReservas->setItem(row, 11, itemStatus);

        exibidas++;
    }

    lblTotalExibido->setText(QString("Exibindo %1 reserva(s) | Arquivo automático: %2")
                                 .arg(exibidas)
                                 .arg(GerenciadorReservas::getInstance().getCaminhoArquivoReservas()));
    aoMudarSelecao();
}

void TelaListagemReservas::aoAlterarFiltros()
{
    carregarTabela();
}

void TelaListagemReservas::aoMudarSelecao()
{
    int linha = tabelaReservas->currentRow();
    bool temSelecao = (linha >= 0);

    btnVerResumo->setEnabled(temSelecao);
    if (!temSelecao) {
        btnConfirmarPagamento->setEnabled(false);
        btnCancelarReserva->setEnabled(false);
        return;
    }

    int statusInt = tabelaReservas->item(linha, 11)->data(Qt::UserRole).toInt();
    StatusReserva status = static_cast<StatusReserva>(statusInt);

    btnConfirmarPagamento->setEnabled(status == StatusReserva::Pendente);
    btnCancelarReserva->setEnabled(status != StatusReserva::Cancelada);
}

void TelaListagemReservas::aoConfirmarPagamento()
{
    int id = idReservaSelecionada();
    if (id < 0) {
        return;
    }

    ResultadoOperacaoReserva res = GerenciadorReservas::getInstance().confirmarPagamento(id);
    if (!res.sucesso) {
        QMessageBox::warning(this, "Não foi possível confirmar", res.mensagem);
        return;
    }

    QString msg = res.mensagem;
    if (!res.reservasCanceladas.isEmpty()) {
        msg += "\n\nReservas pendentes conflitantes canceladas automaticamente:\n";
        for (const Reserva& canc : res.reservasCanceladas) {
            msg += QString("• Reserva #%1 - %2 (%3 a %4)\n")
                       .arg(canc.getId())
                       .arg(canc.getCliente().getNome())
                       .arg(canc.getDataCheckIn().toString("dd/MM/yyyy"))
                       .arg(canc.getDataCheckOut().toString("dd/MM/yyyy"));
        }
    }

    QMessageBox::information(this, "Pagamento confirmado", msg);
    carregarTabela();
}

void TelaListagemReservas::aoCancelarReserva()
{
    int id = idReservaSelecionada();
    if (id < 0) {
        return;
    }

    auto resposta = QMessageBox::question(this,
                                          "Confirmar cancelamento",
                                          QString("Deseja realmente cancelar a reserva #%1?").arg(id));
    if (resposta != QMessageBox::Yes) {
        return;
    }

    ResultadoOperacaoReserva res = GerenciadorReservas::getInstance().cancelarReserva(id);
    if (!res.sucesso) {
        QMessageBox::warning(this, "Erro", res.mensagem);
        return;
    }

    QMessageBox::information(this, "Reserva cancelada", res.mensagem);
    carregarTabela();
}

void TelaListagemReservas::aoVerResumo()
{
    int id = idReservaSelecionada();
    if (id < 0) {
        return;
    }

    auto reservaOpt = GerenciadorReservas::getInstance().buscarReservaPorId(id);
    if (reservaOpt.has_value()) {
        TelaResumoReserva resumo(*reservaOpt, this);
        resumo.exec();
    }
}

void TelaListagemReservas::aoVerClientes()
{
    QDialog dlg(this);
    dlg.setWindowTitle("Clientes Cadastrados - Hotéis Paradise");
    dlg.setMinimumSize(460, 320);

    auto* layout = new QVBoxLayout(&dlg);
    auto* tabela = new QTableWidget(&dlg);
    tabela->setColumnCount(2);
    tabela->setHorizontalHeaderLabels({"Nome Completo", "CPF"});
    tabela->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tabela->setSelectionBehavior(QAbstractItemView::SelectRows);
    tabela->verticalHeader()->setVisible(false);
    tabela->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    tabela->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);

    const QList<Cliente>& clientes = GerenciadorReservas::getInstance().getClientes();
    for (const Cliente& c : clientes) {
        int r = tabela->rowCount();
        tabela->insertRow(r);
        tabela->setItem(r, 0, new QTableWidgetItem(c.getNome()));
        tabela->setItem(r, 1, new QTableWidgetItem(c.getCpf()));
    }

    auto* lblInfo = new QLabel(QString("Total: %1 cliente(s) | Arquivo: %2")
                                   .arg(clientes.size())
                                   .arg(GerenciadorReservas::getInstance().getCaminhoArquivoClientes()),
                               &dlg);
    lblInfo->setWordWrap(true);
    lblInfo->setObjectName("subtitulo");

    auto* btnOk = new QPushButton("Fechar", &dlg);
    connect(btnOk, &QPushButton::clicked, &dlg, &QDialog::accept);

    layout->addWidget(tabela);
    layout->addWidget(lblInfo);
    layout->addWidget(btnOk, 0, Qt::AlignRight);

    dlg.exec();
}

void TelaListagemReservas::aoExportarArquivo()
{
    QString caminho = QFileDialog::getSaveFileName(
        this,
        "Exportar Reservas Ordenadas por Check-in",
        "reservas_paradise.csv",
        "Arquivo CSV (*.csv);;Arquivo de Texto (*.txt)");

    if (caminho.isEmpty()) {
        return;
    }

    if (GerenciadorReservas::getInstance().exportarReservas(caminho)) {
        QMessageBox::information(this, "Exportação concluída", "Reservas salvas com sucesso em:\n" + caminho);
    } else {
        QMessageBox::warning(this, "Erro ao exportar", "Não foi possível gravar o arquivo selecionado.");
    }
}
