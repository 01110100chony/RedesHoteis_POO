#ifndef TIPOSHOTEL_H
#define TIPOSHOTEL_H

#include <QList>
#include <QString>

enum class Unidade
{
    Jericoacoara,
    CanoaQuebrada,
    Cumbuco
};

enum class TipoQuarto
{
    Solteiro,
    Duplo,
    Casal,
    Triplo,
    Quadruplo
};

enum class StatusReserva
{
    Pendente,
    Confirmada,
    Cancelada
};

// Classe auxiliar para centralizar textos, descricoes e valores das unidades e quartos
class InfoHotel
{
public:
    static QList<Unidade> todasUnidades();
    static QString unidadeParaTexto(Unidade u);
    static Unidade textoParaUnidade(const QString& texto, bool* ok = nullptr);

    static QList<TipoQuarto> todosTiposQuarto();
    static QString tipoQuartoParaTexto(TipoQuarto q);
    static TipoQuarto textoParaTipoQuarto(const QString& texto, bool* ok = nullptr);

    static QString descricaoQuarto(TipoQuarto q);
    static double valorDiaria(TipoQuarto q);

    static QString statusParaTexto(StatusReserva s);
    static StatusReserva textoParaStatus(const QString& texto, bool* ok = nullptr);
};

#endif // TIPOSHOTEL_H
