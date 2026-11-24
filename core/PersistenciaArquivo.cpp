#include "PersistenciaArquivo.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>

QString PersistenciaArquivo::limparCampoCsv(const QString& texto)
{
    QString limpo = texto;
    limpo.replace(';', ',');
    limpo.replace('\r', ' ');
    limpo.replace('\n', ' ');
    return limpo.trimmed();
}

bool PersistenciaArquivo::salvarClientesCsv(const QString& caminho, const QList<Cliente>& clientes)
{
    QDir().mkpath(QFileInfo(caminho).absolutePath());

    QFile arquivo(caminho);
    if (!arquivo.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        return false;
    }

    // BOM UTF-8 ajuda o Excel a abrir acentos corretamente
    arquivo.write("\xEF\xBB\xBF");

    QTextStream out(&arquivo);
    out.setEncoding(QStringConverter::Utf8);
    out << "nome;cpf\n";

    for (const Cliente& c : clientes) {
        out << limparCampoCsv(c.getNome()) << ";"
            << limparCampoCsv(c.getCpf()) << "\n";
    }

    return true;
}

QList<Cliente> PersistenciaArquivo::carregarClientesCsv(const QString& caminho)
{
    QList<Cliente> lista;
    QFile arquivo(caminho);
    if (!arquivo.exists() || !arquivo.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return lista;
    }

    QTextStream in(&arquivo);
    in.setEncoding(QStringConverter::Utf8);

    bool primeiraLinha = true;
    while (!in.atEnd()) {
        QString linha = in.readLine();
        if (primeiraLinha) {
            primeiraLinha = false;
            if (linha.startsWith(QChar(0xFEFF))) {
                linha.remove(0, 1);
            }
            if (linha.toLower().startsWith("nome;")) {
                continue;
            }
        }
        if (linha.trimmed().isEmpty()) {
            continue;
        }

        QStringList partes = linha.split(';');
        if (partes.size() >= 2) {
            Cliente c(partes[0], partes[1]);
            if (!c.getNome().isEmpty() && Cliente::validarCpf(c.getCpf())) {
                lista.append(c);
            }
        }
    }

    return lista;
}

bool PersistenciaArquivo::salvarReservasCsv(const QString& caminho, const QList<Reserva>& reservasOrdenadas)
{
    QDir().mkpath(QFileInfo(caminho).absolutePath());

    QFile arquivo(caminho);
    if (!arquivo.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        return false;
    }

    arquivo.write("\xEF\xBB\xBF");

    QTextStream out(&arquivo);
    out.setEncoding(QStringConverter::Utf8);
    out << "id;atendente_usuario;atendente_nome;cliente_nome;cliente_cpf;unidade;quarto;check_in;check_out;diarias;politica_desconto;percentual_desconto;valor_diaria;valor_bruto;valor_desconto;valor_total;valor_entrada;status\n";

    for (const Reserva& r : reservasOrdenadas) {
        out << r.getId() << ";"
            << limparCampoCsv(r.getUsuarioAtendente()) << ";"
            << limparCampoCsv(r.getNomeAtendente()) << ";"
            << limparCampoCsv(r.getCliente().getNome()) << ";"
            << limparCampoCsv(r.getCliente().getCpf()) << ";"
            << limparCampoCsv(InfoHotel::unidadeParaTexto(r.getUnidade())) << ";"
            << limparCampoCsv(InfoHotel::tipoQuartoParaTexto(r.getTipoQuarto())) << ";"
            << r.getDataCheckIn().toString("dd/MM/yyyy") << ";"
            << r.getDataCheckOut().toString("dd/MM/yyyy") << ";"
            << r.getNumDiarias() << ";"
            << limparCampoCsv(r.getNomePoliticaDesconto()) << ";"
            << QString::number(r.getPercentualDesconto(), 'f', 0) << ";"
            << QString::number(r.getValorDiaria(), 'f', 2) << ";"
            << QString::number(r.getValorBruto(), 'f', 2) << ";"
            << QString::number(r.getValorDesconto(), 'f', 2) << ";"
            << QString::number(r.getValorTotal(), 'f', 2) << ";"
            << QString::number(r.getValorEntrada(), 'f', 2) << ";"
            << limparCampoCsv(InfoHotel::statusParaTexto(r.getStatus())) << "\n";
    }

    return true;
}

QList<Reserva> PersistenciaArquivo::carregarReservasCsv(const QString& caminho)
{
    QList<Reserva> lista;
    QFile arquivo(caminho);
    if (!arquivo.exists() || !arquivo.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return lista;
    }

    QTextStream in(&arquivo);
    in.setEncoding(QStringConverter::Utf8);

    bool primeiraLinha = true;
    while (!in.atEnd()) {
        QString linha = in.readLine();
        if (primeiraLinha) {
            primeiraLinha = false;
            if (linha.startsWith(QChar(0xFEFF))) {
                linha.remove(0, 1);
            }
            if (linha.toLower().startsWith("id;")) {
                continue;
            }
        }
        if (linha.trimmed().isEmpty()) {
            continue;
        }

        QStringList p = linha.split(';');
        if (p.size() < 18) {
            continue;
        }

        bool okId = false;
        bool okDiarias = false;
        int id = p[0].toInt(&okId);
        QDate checkIn = QDate::fromString(p[7].trimmed(), "dd/MM/yyyy");
        int diarias = p[9].toInt(&okDiarias);

        if (!okId || !checkIn.isValid() || !okDiarias || diarias <= 0) {
            continue;
        }

        Cliente cliente(p[3], p[4]);
        Unidade unidade = InfoHotel::textoParaUnidade(p[5]);
        TipoQuarto quarto = InfoHotel::textoParaTipoQuarto(p[6]);
        double pctDesc = p[11].toDouble();
        double valDiaria = p[12].toDouble();
        double valBruto = p[13].toDouble();
        double valDesc = p[14].toDouble();
        double valTotal = p[15].toDouble();
        double valEntrada = p[16].toDouble();
        StatusReserva status = InfoHotel::textoParaStatus(p[17]);

        lista.append(Reserva(id,
                             p[1].trimmed(),
                             p[2].trimmed(),
                             cliente,
                             unidade,
                             quarto,
                             checkIn,
                             diarias,
                             p[10].trimmed(),
                             pctDesc,
                             valDiaria,
                             valBruto,
                             valDesc,
                             valTotal,
                             valEntrada,
                             status));
    }

    return lista;
}

bool PersistenciaArquivo::exportarReservasTxt(const QString& caminho, const QList<Reserva>& reservasOrdenadas)
{
    QDir().mkpath(QFileInfo(caminho).absolutePath());

    QFile arquivo(caminho);
    if (!arquivo.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        return false;
    }

    QTextStream out(&arquivo);
    out.setEncoding(QStringConverter::Utf8);

    out << "============================================================\n";
    out << "REDE DE HOTÉIS PARADISE - RELATÓRIO DE RESERVAS\n";
    out << "(Ordenado por data de check-in: mais próxima -> mais distante)\n";
    out << "============================================================\n\n";

    if (reservasOrdenadas.isEmpty()) {
        out << "Nenhuma reserva cadastrada até o momento.\n";
        return true;
    }

    for (const Reserva& r : reservasOrdenadas) {
        out << r.gerarTextoResumo() << "\n";
        out << "------------------------------------------------------------\n";
    }

    return true;
}
