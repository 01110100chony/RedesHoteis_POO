#include "TelaReserva.h"
#include "TelaResumoReserva.h"
#include "core/GerenciadorReservas.h"
#include "model/EstrategiaDesconto.h"

#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLocale>
#include <QMessageBox>
#include <QVBoxLayout>
#include <memory>

TelaReserva::TelaReserva(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("Consulta de Disponibilidade e Reserva - Hotéis Paradise");
    setMinimumSize(740, 710);

    auto* layoutPrincipal = new QVBoxLayout(this);
    layoutPrincipal->setContentsMargins(20, 18, 20, 18);
    layoutPrincipal->setSpacing(14);

    auto* lblTitulo = new QLabel("Consulta de Disponibilidade e Realização de Reserva", this);
    lblTitulo->setObjectName("tituloPrincipal");
    layoutPrincipal->addWidget(lblTitulo);

    // 1. Grupo de consulta de disponibilidade
    auto* grupoPeriodo = new QGroupBox("1. Período e Acomodação", this);
    auto* gridPeriodo = new QGridLayout(grupoPeriodo);
    gridPeriodo->setHorizontalSpacing(12);
    gridPeriodo->setVerticalSpacing(12);

    cmbUnidade = new QComboBox(grupoPeriodo);
    for (Unidade u : InfoHotel::todasUnidades()) {
        cmbUnidade->addItem(InfoHotel::unidadeParaTexto(u), static_cast<int>(u));
    }

    cmbTipoQuarto = new QComboBox(grupoPeriodo);
    QLocale br(QLocale::Portuguese, QLocale::Brazil);
    for (TipoQuarto q : InfoHotel::todosTiposQuarto()) {
        QString texto = QString("%1 (%2/diária)")
                            .arg(InfoHotel::tipoQuartoParaTexto(q))
                            .arg(br.toCurrencyString(InfoHotel::valorDiaria(q), "R$ "));
        cmbTipoQuarto->addItem(texto, static_cast<int>(q));
    }

    lblDescricaoQuarto = new QLabel(grupoPeriodo);
    lblDescricaoQuarto->setStyleSheet("color: #486581; font-style: italic; background: transparent;");
    lblDescricaoQuarto->setWordWrap(true);

    dtCheckIn = new QDateEdit(QDate::currentDate(), grupoPeriodo);
    dtCheckIn->setDisplayFormat("dd/MM/yyyy");
    dtCheckIn->setCalendarPopup(true);

    spnDiarias = new QSpinBox(grupoPeriodo);
    spnDiarias->setRange(1, 365);
    spnDiarias->setValue(1);

    lblCheckOut = new QLabel(grupoPeriodo);
    lblCheckOut->setStyleSheet("color: #334e68; background: transparent;");

    auto* btnConsultar = new QPushButton("Consultar Disponibilidade", grupoPeriodo);
    btnConsultar->setObjectName("btnPrimario");

    lblStatusDisponibilidade = new QLabel("Clique em 'Consultar Disponibilidade' para verificar o período.", grupoPeriodo);
    lblStatusDisponibilidade->setWordWrap(true);
    lblStatusDisponibilidade->setMinimumHeight(34);
    lblStatusDisponibilidade->setStyleSheet("padding: 8px; border-radius: 4px; background-color: #f1f5f9; color: #334e68;");

    gridPeriodo->addWidget(new QLabel("Unidade:", grupoPeriodo), 0, 0);
    gridPeriodo->addWidget(cmbUnidade, 0, 1);
    gridPeriodo->addWidget(new QLabel("Tipo de Quarto:", grupoPeriodo), 0, 2);
    gridPeriodo->addWidget(cmbTipoQuarto, 0, 3);

    gridPeriodo->addWidget(lblDescricaoQuarto, 1, 0, 1, 4);

    gridPeriodo->addWidget(new QLabel("Check-in:", grupoPeriodo), 2, 0);
    gridPeriodo->addWidget(dtCheckIn, 2, 1);
    gridPeriodo->addWidget(new QLabel("Diárias:", grupoPeriodo), 2, 2);
    gridPeriodo->addWidget(spnDiarias, 2, 3);

    gridPeriodo->addWidget(lblCheckOut, 3, 0, 1, 2);
    gridPeriodo->addWidget(btnConsultar, 3, 2, 1, 2);
    gridPeriodo->addWidget(lblStatusDisponibilidade, 4, 0, 1, 4);

    layoutPrincipal->addWidget(grupoPeriodo);

    // 2. Grupo de dados do cliente e desconto
    auto* grupoCliente = new QGroupBox("2. Dados do Cliente e Política de Desconto", this);
    auto* formCliente = new QFormLayout(grupoCliente);
    formCliente->setHorizontalSpacing(12);
    formCliente->setVerticalSpacing(12);

    cmbClientesCadastrados = new QComboBox(grupoCliente);
    carregarClientesNoCombo();

    auto* cpfLayout = new QHBoxLayout();
    txtCpf = new QLineEdit(grupoCliente);
    txtCpf->setPlaceholderText("000.000.000-00");
    auto* btnBuscarCpf = new QPushButton("Buscar CPF", grupoCliente);
    cpfLayout->addWidget(txtCpf);
    cpfLayout->addWidget(btnBuscarCpf);

    txtNomeCliente = new QLineEdit(grupoCliente);
    txtNomeCliente->setPlaceholderText("Nome completo do cliente");

    cmbDesconto = new QComboBox(grupoCliente);
    cmbDesconto->addItems(EstrategiaDesconto::listarOpcoes());

    chkPagamentoConfirmado = new QCheckBox("Pagamento da entrada (1/3) já efetuado (confirmar reserva agora)", grupoCliente);
    chkPagamentoConfirmado->setStyleSheet("background: transparent;");

    formCliente->addRow("Clientes cadastrados:", cmbClientesCadastrados);
    formCliente->addRow("CPF:", cpfLayout);
    formCliente->addRow("Nome completo:", txtNomeCliente);
    formCliente->addRow("Política de desconto:", cmbDesconto);
    formCliente->addRow("", chkPagamentoConfirmado);

    layoutPrincipal->addWidget(grupoCliente);

    // 3. Previa dos valores
    auto* grupoValores = new QGroupBox("3. Prévia dos Valores", this);
    auto* gridValores = new QGridLayout(grupoValores);
    gridValores->setHorizontalSpacing(12);
    gridValores->setVerticalSpacing(10);

    lblValorBruto = new QLabel(grupoValores);
    lblValorDesconto = new QLabel(grupoValores);
    lblValorTotal = new QLabel(grupoValores);
    lblValorTotal->setStyleSheet("font-weight: bold; font-size: 14px; color: #0d3b66; background: transparent;");
    lblValorEntrada = new QLabel(grupoValores);
    lblValorEntrada->setStyleSheet("font-weight: bold; color: #2e7d32; background: transparent;");

    gridValores->addWidget(new QLabel("Valor sem desconto:", grupoValores), 0, 0);
    gridValores->addWidget(lblValorBruto, 0, 1);
    gridValores->addWidget(new QLabel("Desconto:", grupoValores), 0, 2);
    gridValores->addWidget(lblValorDesconto, 0, 3);
    gridValores->addWidget(new QLabel("Valor total:", grupoValores), 1, 0);
    gridValores->addWidget(lblValorTotal, 1, 1);
    gridValores->addWidget(new QLabel("Entrada (1/3):", grupoValores), 1, 2);
    gridValores->addWidget(lblValorEntrada, 1, 3);

    layoutPrincipal->addWidget(grupoValores);

    // Botoes finais
    auto* botoesLayout = new QHBoxLayout();
    botoesLayout->addStretch();

    auto* btnVoltar = new QPushButton("Voltar", this);
    auto* btnConfirmar = new QPushButton("Registrar Reserva", this);
    btnConfirmar->setObjectName("btnSucesso");
    btnConfirmar->setMinimumHeight(36);

    botoesLayout->addWidget(btnVoltar);
    botoesLayout->addWidget(btnConfirmar);
    layoutPrincipal->addLayout(botoesLayout);

    // Sinais e slots
    connect(cmbTipoQuarto, &QComboBox::currentIndexChanged, this, &TelaReserva::aoAlterarTipoQuarto);
    connect(dtCheckIn, &QDateEdit::dateChanged, this, &TelaReserva::aoAlterarPeriodoOuDesconto);
    connect(spnDiarias, &QSpinBox::valueChanged, this, &TelaReserva::aoAlterarPeriodoOuDesconto);
    connect(cmbDesconto, &QComboBox::currentIndexChanged, this, &TelaReserva::aoAlterarPeriodoOuDesconto);

    connect(cmbClientesCadastrados, &QComboBox::currentIndexChanged, this, &TelaReserva::aoSelecionarClienteCombo);
    connect(btnBuscarCpf, &QPushButton::clicked, this, &TelaReserva::aoBuscarClientePorCpf);
    connect(btnConsultar, &QPushButton::clicked, this, &TelaReserva::aoConsultarDisponibilidade);
    connect(btnConfirmar, &QPushButton::clicked, this, &TelaReserva::aoConfirmarReserva);
    connect(btnVoltar, &QPushButton::clicked, this, &QDialog::reject);

    aoAlterarTipoQuarto();
    aoAlterarPeriodoOuDesconto();
}

Unidade TelaReserva::unidadeSelecionada() const
{
    return static_cast<Unidade>(cmbUnidade->currentData().toInt());
}

TipoQuarto TelaReserva::quartoSelecionado() const
{
    return static_cast<TipoQuarto>(cmbTipoQuarto->currentData().toInt());
}

void TelaReserva::carregarClientesNoCombo()
{
    cmbClientesCadastrados->clear();
    cmbClientesCadastrados->addItem("(Novo cliente / digitar dados)", "");

    const QList<Cliente>& lista = GerenciadorReservas::getInstance().getClientes();
    for (const Cliente& c : lista) {
        cmbClientesCadastrados->addItem(c.getResumoIdentificacao(), c.getCpfNumerico());
    }
}

void TelaReserva::aoAlterarTipoQuarto()
{
    lblDescricaoQuarto->setText("Descrição: " + InfoHotel::descricaoQuarto(quartoSelecionado()));
    aoAlterarPeriodoOuDesconto();
}

void TelaReserva::aoAlterarPeriodoOuDesconto()
{
    QDate checkIn = dtCheckIn->date();
    int diarias = spnDiarias->value();
    QDate checkOut = checkIn.addDays(diarias);
    lblCheckOut->setText("Check-out previsto: " + checkOut.toString("dd/MM/yyyy"));

    std::unique_ptr<EstrategiaDesconto> estrategia = EstrategiaDesconto::criarPorIndice(cmbDesconto->currentIndex());
    CalculoReserva calc = GerenciadorReservas::getInstance().calcularValores(quartoSelecionado(), diarias, *estrategia);

    QLocale br(QLocale::Portuguese, QLocale::Brazil);
    lblValorBruto->setText(br.toCurrencyString(calc.valorBruto, "R$ "));
    lblValorDesconto->setText(QString("%1 (%2%)")
                                  .arg(br.toCurrencyString(calc.valorDesconto, "R$ "))
                                  .arg(calc.percentualDesconto, 0, 'f', 0));
    lblValorTotal->setText(br.toCurrencyString(calc.valorTotal, "R$ "));
    lblValorEntrada->setText(br.toCurrencyString(calc.valorEntrada, "R$ "));
}

void TelaReserva::aoSelecionarClienteCombo(int indice)
{
    if (indice <= 0) {
        return;
    }

    QString cpf = cmbClientesCadastrados->itemData(indice).toString();
    auto clienteOpt = GerenciadorReservas::getInstance().buscarClientePorCpf(cpf);
    if (clienteOpt.has_value()) {
        txtCpf->setText(clienteOpt->getCpf());
        txtNomeCliente->setText(clienteOpt->getNome());
    }
}

void TelaReserva::aoBuscarClientePorCpf()
{
    auto clienteOpt = GerenciadorReservas::getInstance().buscarClientePorCpf(txtCpf->text());
    if (clienteOpt.has_value()) {
        txtCpf->setText(clienteOpt->getCpf());
        txtNomeCliente->setText(clienteOpt->getNome());
        QMessageBox::information(this, "Cliente encontrado", "Dados preenchidos a partir do cadastro de clientes.");
    } else {
        QMessageBox::information(this, "Cliente não encontrado", "Nenhum cliente cadastrado com este CPF. Preencha o nome para cadastrá-lo.");
    }
}

void TelaReserva::aoConsultarDisponibilidade()
{
    ResultadoDisponibilidade disp = GerenciadorReservas::getInstance().consultarDisponibilidade(
        unidadeSelecionada(),
        quartoSelecionado(),
        dtCheckIn->date(),
        spnDiarias->value());

    lblStatusDisponibilidade->setText(disp.mensagem);

    if (!disp.disponivel) {
        lblStatusDisponibilidade->setStyleSheet("padding: 8px; border-radius: 4px; background-color: #ffebee; color: #b71c1c; font-weight: bold;");
        QMessageBox::warning(this, "Indisponível", disp.mensagem);
    } else if (disp.possuiPendentes) {
        lblStatusDisponibilidade->setStyleSheet("padding: 8px; border-radius: 4px; background-color: #fff3e0; color: #e65100; font-weight: bold;");
    } else {
        lblStatusDisponibilidade->setStyleSheet("padding: 8px; border-radius: 4px; background-color: #e8f5e9; color: #1b5e20; font-weight: bold;");
    }
}

void TelaReserva::aoConfirmarReserva()
{
    std::unique_ptr<EstrategiaDesconto> estrategia = EstrategiaDesconto::criarPorIndice(cmbDesconto->currentIndex());

    ResultadoOperacaoReserva res = GerenciadorReservas::getInstance().criarReserva(
        txtNomeCliente->text(),
        txtCpf->text(),
        unidadeSelecionada(),
        quartoSelecionado(),
        dtCheckIn->date(),
        spnDiarias->value(),
        *estrategia,
        chkPagamentoConfirmado->isChecked());

    if (!res.sucesso) {
        QMessageBox::warning(this, "Não foi possível registrar", res.mensagem);
        return;
    }

    if (!res.reservasCanceladas.isEmpty()) {
        QString aviso = "As seguintes reservas pendentes conflitantes foram canceladas automaticamente após a confirmação do pagamento:\n\n";
        for (const Reserva& canc : res.reservasCanceladas) {
            aviso += QString("• Reserva #%1 - %2 (%3 a %4)\n")
                         .arg(canc.getId())
                         .arg(canc.getCliente().getNome())
                         .arg(canc.getDataCheckIn().toString("dd/MM/yyyy"))
                         .arg(canc.getDataCheckOut().toString("dd/MM/yyyy"));
        }
        QMessageBox::information(this, "Cancelamento por conflito", aviso);
    }

    auto criada = GerenciadorReservas::getInstance().buscarReservaPorId(res.idReserva);
    if (criada.has_value()) {
        TelaResumoReserva resumo(*criada, this);
        resumo.exec();
    }

    accept();
}
