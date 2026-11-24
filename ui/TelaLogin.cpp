#include "TelaLogin.h"
#include "core/GerenciadorReservas.h"

#include <QFormLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QVBoxLayout>

TelaLogin::TelaLogin(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("Login - Rede de Hotéis Paradise");
    setMinimumWidth(400);

    auto* layoutPrincipal = new QVBoxLayout(this);
    layoutPrincipal->setContentsMargins(24, 24, 24, 24);
    layoutPrincipal->setSpacing(14);

    auto* lblTitulo = new QLabel("Rede de Hotéis Paradise", this);
    lblTitulo->setObjectName("tituloPrincipal");
    lblTitulo->setAlignment(Qt::AlignCenter);

    auto* lblSubtitulo = new QLabel("Sistema de Reservas por Telefone\nAcesso restrito aos atendentes", this);
    lblSubtitulo->setObjectName("subtitulo");
    lblSubtitulo->setAlignment(Qt::AlignCenter);

    layoutPrincipal->addWidget(lblTitulo);
    layoutPrincipal->addWidget(lblSubtitulo);

    auto* formLayout = new QFormLayout();
    formLayout->setSpacing(10);

    txtUsuario = new QLineEdit(this);
    txtUsuario->setPlaceholderText("Ex: atendente1");
    txtSenha = new QLineEdit(this);
    txtSenha->setEchoMode(QLineEdit::Password);
    txtSenha->setPlaceholderText("Digite sua senha");

    formLayout->addRow("Atendente:", txtUsuario);
    formLayout->addRow("Senha:", txtSenha);
    layoutPrincipal->addLayout(formLayout);

    lblErro = new QLabel(this);
    lblErro->setStyleSheet("color: #c62828; font-weight: bold;");
    lblErro->setWordWrap(true);
    lblErro->setVisible(false);
    layoutPrincipal->addWidget(lblErro);

    auto* botoesLayout = new QHBoxLayout();
    botoesLayout->addStretch();

    auto* btnCancelar = new QPushButton("Cancelar", this);
    btnEntrar = new QPushButton("Entrar", this);
    btnEntrar->setObjectName("btnPrimario");
    btnEntrar->setDefault(true);

    botoesLayout->addWidget(btnCancelar);
    botoesLayout->addWidget(btnEntrar);
    layoutPrincipal->addLayout(botoesLayout);

    connect(btnEntrar, &QPushButton::clicked, this, &TelaLogin::aoClicarEntrar);
    connect(btnCancelar, &QPushButton::clicked, this, &QDialog::reject);

    txtUsuario->setFocus();
}

void TelaLogin::aoClicarEntrar()
{
    QString usuario = txtUsuario->text().trimmed();
    QString senha = txtSenha->text();

    if (usuario.isEmpty() || senha.isEmpty()) {
        lblErro->setText("Preencha usuário e senha para continuar.");
        lblErro->setVisible(true);
        QMessageBox::warning(this, "Campos obrigatórios", "Informe o usuário e a senha do atendente.");
        return;
    }

    if (GerenciadorReservas::getInstance().autenticar(usuario, senha)) {
        lblErro->setVisible(false);
        accept();
    } else {
        lblErro->setText("Usuário ou senha inválidos. Tente novamente.");
        lblErro->setVisible(true);
        QMessageBox::warning(this, "Falha na autenticação", "Usuário ou senha inválidos.");
        txtSenha->clear();
        txtSenha->setFocus();
    }
}
