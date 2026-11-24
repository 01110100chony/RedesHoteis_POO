#include "TelaResumoReserva.h"

#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLocale>
#include <QPushButton>
#include <QVBoxLayout>

TelaResumoReserva::TelaResumoReserva(const Reserva& reserva, QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(QString("Resumo da Reserva #%1 - Hotéis Paradise").arg(reserva.getId()));
    setMinimumWidth(480);

    QLocale br(QLocale::Portuguese, QLocale::Brazil);

    auto* layoutPrincipal = new QVBoxLayout(this);
    layoutPrincipal->setContentsMargins(20, 20, 20, 20);
    layoutPrincipal->setSpacing(14);

    auto* lblTitulo = new QLabel(QString("Resumo da Reserva #%1").arg(reserva.getId()), this);
    lblTitulo->setObjectName("tituloPrincipal");
    layoutPrincipal->addWidget(lblTitulo);

    auto* lblBadgeStatus = new QLabel(InfoHotel::statusParaTexto(reserva.getStatus()), this);
    lblBadgeStatus->setAlignment(Qt::AlignCenter);

    QString estiloStatus = "padding: 6px 12px; border-radius: 4px; font-weight: bold;";
    if (reserva.getStatus() == StatusReserva::Confirmada) {
        estiloStatus += "background-color: #e8f5e9; color: #1b5e20; border: 1px solid #81c784;";
    } else if (reserva.getStatus() == StatusReserva::Pendente) {
        estiloStatus += "background-color: #fff3e0; color: #e65100; border: 1px solid #ffb74d;";
    } else {
        estiloStatus += "background-color: #ffebee; color: #b71c1c; border: 1px solid #e57373;";
    }
    lblBadgeStatus->setStyleSheet(estiloStatus);
    layoutPrincipal->addWidget(lblBadgeStatus);

    auto* grupoDados = new QGroupBox("Dados da Hospedagem e Valores", this);
    auto* form = new QFormLayout(grupoDados);
    form->setSpacing(8);

    auto criarValorNegrito = [&](const QString& texto) {
        auto* lbl = new QLabel(texto, grupoDados);
        lbl->setWordWrap(true);
        lbl->setStyleSheet("font-weight: 600; background: transparent;");
        return lbl;
    };

    form->addRow("Atendente:", criarValorNegrito(QString("%1 (%2)").arg(reserva.getNomeAtendente(), reserva.getUsuarioAtendente())));
    form->addRow("Cliente:", criarValorNegrito(reserva.getCliente().getNome()));
    form->addRow("CPF do cliente:", criarValorNegrito(reserva.getCliente().getCpf()));
    form->addRow("Localidade:", criarValorNegrito(InfoHotel::unidadeParaTexto(reserva.getUnidade())));
    form->addRow("Tipo de quarto:", criarValorNegrito(QString("%1 (diária %2)")
                                                        .arg(InfoHotel::tipoQuartoParaTexto(reserva.getTipoQuarto()))
                                                        .arg(br.toCurrencyString(reserva.getValorDiaria(), "R$ "))));
    form->addRow("Período:", criarValorNegrito(QString("%1 a %2 (%3 diária(s))")
                                                 .arg(reserva.getDataCheckIn().toString("dd/MM/yyyy"))
                                                 .arg(reserva.getDataCheckOut().toString("dd/MM/yyyy"))
                                                 .arg(reserva.getNumDiarias())));
    form->addRow("Política de desconto:", criarValorNegrito(QString("%1 (%2%)")
                                                              .arg(reserva.getNomePoliticaDesconto())
                                                              .arg(reserva.getPercentualDesconto(), 0, 'f', 0)));
    form->addRow("Valor sem desconto:", criarValorNegrito(br.toCurrencyString(reserva.getValorBruto(), "R$ ")));
    form->addRow("Desconto aplicado:", criarValorNegrito(br.toCurrencyString(reserva.getValorDesconto(), "R$ ")));
    form->addRow("Valor total:", criarValorNegrito(br.toCurrencyString(reserva.getValorTotal(), "R$ ")));
    form->addRow("Entrada (1/3):", criarValorNegrito(br.toCurrencyString(reserva.getValorEntrada(), "R$ ")));

    layoutPrincipal->addWidget(grupoDados);

    auto* botoes = new QHBoxLayout();
    botoes->addStretch();
    auto* btnFechar = new QPushButton("Fechar", this);
    btnFechar->setObjectName("btnPrimario");
    btnFechar->setDefault(true);
    botoes->addWidget(btnFechar);
    layoutPrincipal->addLayout(botoes);

    connect(btnFechar, &QPushButton::clicked, this, &QDialog::accept);
}
