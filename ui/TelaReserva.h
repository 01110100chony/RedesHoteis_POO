#ifndef TELARESERVA_H
#define TELARESERVA_H

#include "model/TiposHotel.h"
#include <QCheckBox>
#include <QComboBox>
#include <QDateEdit>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>

class TelaReserva : public QDialog
{
    Q_OBJECT

private:
    QComboBox* cmbUnidade = nullptr;
    QComboBox* cmbTipoQuarto = nullptr;
    QLabel* lblDescricaoQuarto = nullptr;
    QDateEdit* dtCheckIn = nullptr;
    QSpinBox* spnDiarias = nullptr;
    QLabel* lblCheckOut = nullptr;
    QLabel* lblStatusDisponibilidade = nullptr;

    QComboBox* cmbClientesCadastrados = nullptr;
    QLineEdit* txtCpf = nullptr;
    QLineEdit* txtNomeCliente = nullptr;
    QComboBox* cmbDesconto = nullptr;
    QCheckBox* chkPagamentoConfirmado = nullptr;

    QLabel* lblValorBruto = nullptr;
    QLabel* lblValorDesconto = nullptr;
    QLabel* lblValorTotal = nullptr;
    QLabel* lblValorEntrada = nullptr;

    Unidade unidadeSelecionada() const;
    TipoQuarto quartoSelecionado() const;
    void carregarClientesNoCombo();

private slots:
    void aoAlterarTipoQuarto();
    void aoAlterarPeriodoOuDesconto();
    void aoSelecionarClienteCombo(int indice);
    void aoBuscarClientePorCpf();
    void aoConsultarDisponibilidade();
    void aoConfirmarReserva();

public:
    explicit TelaReserva(QWidget* parent = nullptr);
};

#endif // TELARESERVA_H
