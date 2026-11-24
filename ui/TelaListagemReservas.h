#ifndef TELALISTAGEMRESERVAS_H
#define TELALISTAGEMRESERVAS_H

#include "model/Reserva.h"
#include <QComboBox>
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>

class TelaListagemReservas : public QDialog
{
    Q_OBJECT

private:
    QComboBox* cmbFiltroUnidade = nullptr;
    QComboBox* cmbFiltroStatus = nullptr;
    QTableWidget* tabelaReservas = nullptr;
    QLabel* lblTotalExibido = nullptr;

    QPushButton* btnConfirmarPagamento = nullptr;
    QPushButton* btnCancelarReserva = nullptr;
    QPushButton* btnVerResumo = nullptr;

    int idReservaSelecionada() const;
    void carregarTabela();

private slots:
    void aoAlterarFiltros();
    void aoMudarSelecao();
    void aoConfirmarPagamento();
    void aoCancelarReserva();
    void aoVerResumo();
    void aoVerClientes();
    void aoExportarArquivo();

public:
    explicit TelaListagemReservas(QWidget* parent = nullptr);
};

#endif // TELALISTAGEMRESERVAS_H
