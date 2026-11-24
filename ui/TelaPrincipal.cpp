#include "TelaPrincipal.h"
#include "TelaListagemReservas.h"
#include "TelaReserva.h"
#include "core/GerenciadorReservas.h"

#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QWidget>

TelaPrincipal::TelaPrincipal(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("Rede de Hotéis Paradise - Painel Principal");
    setMinimumSize(620, 440);

    montarMenu();
    montarLayout();
    atualizarResumoPainel();
}

void TelaPrincipal::montarMenu()
{
    QMenu* menuSistema = menuBar()->addMenu("Sistema");
    QAction* acaoReserva = menuSistema->addAction("Consultar Disponibilidade / Nova Reserva");
    QAction* acaoListagem = menuSistema->addAction("Listar Reservas");
    menuSistema->addSeparator();
    QAction* acaoSair = menuSistema->addAction("Sair do Sistema");

    connect(acaoReserva, &QAction::triggered, this, &TelaPrincipal::abrirTelaReserva);
    connect(acaoListagem, &QAction::triggered, this, &TelaPrincipal::abrirTelaListagem);
    connect(acaoSair, &QAction::triggered, this, &TelaPrincipal::sairSistema);
}

void TelaPrincipal::montarLayout()
{
    auto* central = new QWidget(this);
    setCentralWidget(central);

    auto* layoutPrincipal = new QVBoxLayout(central);
    layoutPrincipal->setContentsMargins(28, 24, 28, 24);
    layoutPrincipal->setSpacing(18);

    auto* lblTitulo = new QLabel("Rede de Hotéis Paradise", central);
    lblTitulo->setObjectName("tituloPrincipal");

    QString textoAtendente = "Atendente não identificado";
    if (const Atendente* a = GerenciadorReservas::getInstance().getAtendenteLogado()) {
        textoAtendente = "Atendente autenticado: " + a->getResumoIdentificacao();
    }
    lblAtendente = new QLabel(textoAtendente, central);
    lblAtendente->setObjectName("subtitulo");

    layoutPrincipal->addWidget(lblTitulo);
    layoutPrincipal->addWidget(lblAtendente);

    // Cartoes simples com o resumo atual das reservas
    auto* cardsLayout = new QHBoxLayout();
    cardsLayout->setSpacing(12);

    auto criarCard = [&](const QString& titulo, QLabel*& valorLabel, const QString& corBorda) {
        auto* frame = new QFrame(central);
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setStyleSheet(QString("QFrame { background: #ffffff; border: 1px solid #d5e0ea; "
                                     "border-left: 4px solid %1; border-radius: 6px; padding: 10px; }")
                                 .arg(corBorda));
        auto* l = new QVBoxLayout(frame);
        auto* lblTit = new QLabel(titulo, frame);
        lblTit->setStyleSheet("color: #546e7a; font-size: 12px; border: none;");
        valorLabel = new QLabel("0", frame);
        valorLabel->setStyleSheet("color: #0d3b66; font-size: 22px; font-weight: bold; border: none;");
        l->addWidget(lblTit);
        l->addWidget(valorLabel);
        cardsLayout->addWidget(frame);
    };

    criarCard("Total de Reservas", lblTotalReservas, "#0d3b66");
    criarCard("Confirmadas", lblConfirmadas, "#2e7d32");
    criarCard("Pendentes", lblPendentes, "#ef6c00");
    layoutPrincipal->addLayout(cardsLayout);

    auto* lblUnidades = new QLabel("Unidades: Jericoacoara • Canoa Quebrada • Cumbuco", central);
    lblUnidades->setStyleSheet("color: #455a64; font-size: 13px;");
    lblUnidades->setAlignment(Qt::AlignCenter);
    layoutPrincipal->addWidget(lblUnidades);

    // Botoes principais de navegacao
    auto* botoesLayout = new QVBoxLayout();
    botoesLayout->setSpacing(12);

    auto* btnReserva = new QPushButton("Consultar Disponibilidade / Realizar Reserva", central);
    btnReserva->setObjectName("btnPrimario");
    btnReserva->setMinimumHeight(44);

    auto* btnListar = new QPushButton("Listar Reservas e Atualizar Pagamentos", central);
    btnListar->setMinimumHeight(44);

    auto* btnSair = new QPushButton("Sair do Sistema", central);
    btnSair->setMinimumHeight(38);

    botoesLayout->addWidget(btnReserva);
    botoesLayout->addWidget(btnListar);
    botoesLayout->addWidget(btnSair);
    layoutPrincipal->addLayout(botoesLayout);
    layoutPrincipal->addStretch();

    connect(btnReserva, &QPushButton::clicked, this, &TelaPrincipal::abrirTelaReserva);
    connect(btnListar, &QPushButton::clicked, this, &TelaPrincipal::abrirTelaListagem);
    connect(btnSair, &QPushButton::clicked, this, &TelaPrincipal::sairSistema);
}

void TelaPrincipal::atualizarResumoPainel()
{
    const QList<Reserva> lista = GerenciadorReservas::getInstance().listarReservasOrdenadas();
    int total = 0;
    int confirmadas = 0;
    int pendentes = 0;

    for (const Reserva& r : lista) {
        if (r.getStatus() == StatusReserva::Cancelada) {
            continue;
        }
        total++;
        if (r.getStatus() == StatusReserva::Confirmada) {
            confirmadas++;
        } else if (r.getStatus() == StatusReserva::Pendente) {
            pendentes++;
        }
    }

    lblTotalReservas->setText(QString::number(total));
    lblConfirmadas->setText(QString::number(confirmadas));
    lblPendentes->setText(QString::number(pendentes));
}

void TelaPrincipal::abrirTelaReserva()
{
    TelaReserva tela(this);
    tela.exec();
    atualizarResumoPainel();
}

void TelaPrincipal::abrirTelaListagem()
{
    TelaListagemReservas tela(this);
    tela.exec();
    atualizarResumoPainel();
}

void TelaPrincipal::sairSistema()
{
    GerenciadorReservas::getInstance().logout();
    close();
}
