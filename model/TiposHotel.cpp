#include "TiposHotel.h"

QList<Unidade> InfoHotel::todasUnidades()
{
    return {
        Unidade::Jericoacoara,
        Unidade::CanoaQuebrada,
        Unidade::Cumbuco
    };
}

QString InfoHotel::unidadeParaTexto(Unidade u)
{
    switch (u) {
    case Unidade::Jericoacoara:  return "Jericoacoara";
    case Unidade::CanoaQuebrada: return "Canoa Quebrada";
    case Unidade::Cumbuco:       return "Cumbuco";
    }
    return "Jericoacoara";
}

Unidade InfoHotel::textoParaUnidade(const QString& texto, bool* ok)
{
    QString t = texto.trimmed().toLower();
    if (ok) *ok = true;

    if (t == "jericoacoara") return Unidade::Jericoacoara;
    if (t == "canoa quebrada" || t == "canoaquebrada") return Unidade::CanoaQuebrada;
    if (t == "cumbuco") return Unidade::Cumbuco;

    if (ok) *ok = false;
    return Unidade::Jericoacoara;
}

QList<TipoQuarto> InfoHotel::todosTiposQuarto()
{
    return {
        TipoQuarto::Solteiro,
        TipoQuarto::Duplo,
        TipoQuarto::Casal,
        TipoQuarto::Triplo,
        TipoQuarto::Quadruplo
    };
}

QString InfoHotel::tipoQuartoParaTexto(TipoQuarto q)
{
    switch (q) {
    case TipoQuarto::Solteiro:  return "Solteiro";
    case TipoQuarto::Duplo:     return "Duplo";
    case TipoQuarto::Casal:     return "Casal";
    case TipoQuarto::Triplo:    return "Triplo";
    case TipoQuarto::Quadruplo: return "Quádruplo";
    }
    return "Solteiro";
}

TipoQuarto InfoHotel::textoParaTipoQuarto(const QString& texto, bool* ok)
{
    QString t = texto.trimmed().toLower();
    if (ok) *ok = true;

    if (t == "solteiro") return TipoQuarto::Solteiro;
    if (t == "duplo") return TipoQuarto::Duplo;
    if (t == "casal") return TipoQuarto::Casal;
    if (t == "triplo") return TipoQuarto::Triplo;
    if (t == "quádruplo" || t == "quadruplo") return TipoQuarto::Quadruplo;

    if (ok) *ok = false;
    return TipoQuarto::Solteiro;
}

QString InfoHotel::descricaoQuarto(TipoQuarto q)
{
    switch (q) {
    case TipoQuarto::Solteiro:
        return "Ideal para viajantes individuais";
    case TipoQuarto::Duplo:
        return "Duas camas separadas, para amigos ou colegas";
    case TipoQuarto::Casal:
        return "Uma cama de casal, ideal para casais ou famílias pequenas";
    case TipoQuarto::Triplo:
        return "Três camas (uma de casal e uma de solteiro, ou três camas de solteiro)";
    case TipoQuarto::Quadruplo:
        return "Quatro camas (duas de casal ou quatro de solteiro)";
    }
    return "";
}

double InfoHotel::valorDiaria(TipoQuarto q)
{
    switch (q) {
    case TipoQuarto::Solteiro:  return 200.0;
    case TipoQuarto::Duplo:     return 300.0;
    case TipoQuarto::Casal:     return 350.0;
    case TipoQuarto::Triplo:    return 450.0;
    case TipoQuarto::Quadruplo: return 550.0;
    }
    return 0.0;
}

QString InfoHotel::statusParaTexto(StatusReserva s)
{
    switch (s) {
    case StatusReserva::Pendente:   return "Pendente de confirmação";
    case StatusReserva::Confirmada: return "Confirmada";
    case StatusReserva::Cancelada:  return "Cancelada";
    }
    return "Pendente de confirmação";
}

StatusReserva InfoHotel::textoParaStatus(const QString& texto, bool* ok)
{
    QString t = texto.trimmed().toLower();
    if (ok) *ok = true;

    if (t.startsWith("pendente")) return StatusReserva::Pendente;
    if (t == "confirmada") return StatusReserva::Confirmada;
    if (t == "cancelada") return StatusReserva::Cancelada;

    if (ok) *ok = false;
    return StatusReserva::Pendente;
}
