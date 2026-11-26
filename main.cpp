#include "core/GerenciadorReservas.h"
#include "ui/TelaLogin.h"
#include "ui/TelaPrincipal.h"

#include <QApplication>

static QString estiloPadraoSistema()
{
    return R"(
        QWidget {
            font-family: "Segoe UI", Arial, sans-serif;
            font-size: 13px;
            color: #1f2933;
        }
        QMainWindow, QDialog {
            background-color: #f4f7fa;
        }
        QLabel, QCheckBox {
            background-color: transparent;
        }
        QLabel#tituloPrincipal {
            font-size: 20px;
            font-weight: bold;
            color: #0d3b66;
        }
        QLabel#subtitulo {
            font-size: 13px;
            color: #486581;
        }
        QGroupBox {
            font-weight: bold;
            color: #0d3b66;
            border: 1px solid #cbd5e1;
            border-radius: 6px;
            margin-top: 12px;
            padding: 18px 12px 12px 12px;
            background-color: #ffffff;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 12px;
            padding: 0 6px;
            background-color: #f4f7fa;
        }
        QLineEdit, QComboBox, QDateEdit, QSpinBox {
            background-color: #ffffff;
            border: 1px solid #bcccdc;
            border-radius: 4px;
            padding: 5px 8px;
            min-height: 24px;
        }
        QLineEdit:focus, QComboBox:focus, QDateEdit:focus, QSpinBox:focus {
            border: 1px solid #1976d2;
        }
        QPushButton {
            background-color: #e2e8f0;
            border: 1px solid #cbd5e1;
            border-radius: 5px;
            padding: 7px 14px;
            min-height: 22px;
            font-weight: 600;
            color: #1f2933;
        }
        QPushButton:hover {
            background-color: #cbd5e1;
        }
        QPushButton#btnPrimario {
            background-color: #0d47a1;
            border: 1px solid #0b3c8a;
            color: #ffffff;
        }
        QPushButton#btnPrimario:hover {
            background-color: #1565c0;
        }
        QPushButton#btnSucesso {
            background-color: #2e7d32;
            border: 1px solid #256628;
            color: #ffffff;
        }
        QPushButton#btnSucesso:hover {
            background-color: #388e3c;
        }
        QPushButton:disabled {
            background-color: #e5e7eb;
            color: #9ca3af;
            border-color: #d1d5db;
        }
        QTableWidget {
            background-color: #ffffff;
            border: 1px solid #cbd5e1;
            gridline-color: #e2e8f0;
        }
        QHeaderView::section {
            background-color: #0d3b66;
            color: #ffffff;
            padding: 6px;
            border: none;
            font-weight: bold;
        }
    )";
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setOrganizationName("HotelParadise");
    app.setApplicationName("HotelParadise");
    app.setApplicationDisplayName("Hotel Paradise");
    app.setStyle("Fusion");
    app.setStyleSheet(estiloPadraoSistema());

    // Inicializa o Singleton ja com o caminho padrao de dados
    GerenciadorReservas::getInstance();

    TelaLogin login;
    if (login.exec() != QDialog::Accepted) {
        return 0;
    }

    TelaPrincipal principal;
    principal.show();

    return app.exec();
}
