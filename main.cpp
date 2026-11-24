#include <QApplication>
#include <QLabel>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QLabel janela("Sistema de Reservas - Rede de Hoteis Paradise");
    janela.setAlignment(Qt::AlignCenter);
    janela.resize(420, 160);
    janela.show();

    return app.exec();
}
