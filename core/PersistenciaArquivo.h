#ifndef PERSISTENCIAARQUIVO_H
#define PERSISTENCIAARQUIVO_H

#include "model/Cliente.h"
#include "model/Reserva.h"
#include <QList>
#include <QString>

class PersistenciaArquivo
{
private:
    static QString limparCampoCsv(const QString& texto);

public:
    static bool salvarClientesCsv(const QString& caminho, const QList<Cliente>& clientes);
    static QList<Cliente> carregarClientesCsv(const QString& caminho);

    static bool salvarReservasCsv(const QString& caminho, const QList<Reserva>& reservasOrdenadas);
    static QList<Reserva> carregarReservasCsv(const QString& caminho);

    static bool exportarReservasTxt(const QString& caminho, const QList<Reserva>& reservasOrdenadas);
};

#endif // PERSISTENCIAARQUIVO_H
