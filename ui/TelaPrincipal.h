#ifndef TELAPRINCIPAL_H
#define TELAPRINCIPAL_H

#include <QLabel>
#include <QMainWindow>
#include <QPushButton>

class TelaPrincipal : public QMainWindow
{
    Q_OBJECT

private:
    QLabel* lblAtendente = nullptr;
    QLabel* lblTotalReservas = nullptr;
    QLabel* lblConfirmadas = nullptr;
    QLabel* lblPendentes = nullptr;

    void montarMenu();
    void montarLayout();
    void atualizarResumoPainel();

private slots:
    void abrirTelaReserva();
    void abrirTelaListagem();
    void sairSistema();

public:
    explicit TelaPrincipal(QWidget* parent = nullptr);
};

#endif // TELAPRINCIPAL_H
