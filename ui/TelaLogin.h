#ifndef TELALOGIN_H
#define TELALOGIN_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

class TelaLogin : public QDialog
{
    Q_OBJECT

private:
    QLineEdit* txtUsuario = nullptr;
    QLineEdit* txtSenha = nullptr;
    QLabel* lblErro = nullptr;
    QPushButton* btnEntrar = nullptr;

private slots:
    void aoClicarEntrar();

public:
    explicit TelaLogin(QWidget* parent = nullptr);
};

#endif // TELALOGIN_H
