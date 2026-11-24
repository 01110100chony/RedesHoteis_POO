#include "Reserva.h"
#include <QLocale>

Reserva::Reserva(int id,
                 const QString& usuarioAtendente,
                 const QString& nomeAtendente,
                 const Cliente& cliente,
                 Unidade unidade,
                 TipoQuarto tipoQuarto,
                 const QDate& dataCheckIn,
                 int numDiarias,
                 const QString& nomePoliticaDesconto,
                 double percentualDesconto,
                 double valorDiaria,
                 double valorBruto,
                 double valorDesconto,
                 double valorTotal,
                 double valorEntrada,
                 StatusReserva status)
    : id(id),
      usuarioAtendente(usuarioAtendente),
      nomeAtendente(nomeAtendente),
      cliente(cliente),
      unidade(unidade),
      tipoQuarto(tipoQuarto),
      dataCheckIn(dataCheckIn),
      numDiarias(numDiarias),
      nomePoliticaDesconto(nomePoliticaDesconto),
      percentualDesconto(percentualDesconto),
      valorDiaria(valorDiaria),
      valorBruto(valorBruto),
      valorDesconto(valorDesconto),
      valorTotal(valorTotal),
      valorEntrada(valorEntrada),
      status(status)
{
}

int Reserva::getId() const { return id; }
QString Reserva::getUsuarioAtendente() const { return usuarioAtendente; }
QString Reserva::getNomeAtendente() const { return nomeAtendente; }
Cliente Reserva::getCliente() const { return cliente; }
Unidade Reserva::getUnidade() const { return unidade; }
TipoQuarto Reserva::getTipoQuarto() const { return tipoQuarto; }
QDate Reserva::getDataCheckIn() const { return dataCheckIn; }
QDate Reserva::getDataCheckOut() const { return dataCheckIn.addDays(numDiarias); }
int Reserva::getNumDiarias() const { return numDiarias; }
QString Reserva::getNomePoliticaDesconto() const { return nomePoliticaDesconto; }
double Reserva::getPercentualDesconto() const { return percentualDesconto; }
double Reserva::getValorDiaria() const { return valorDiaria; }
double Reserva::getValorBruto() const { return valorBruto; }
double Reserva::getValorDesconto() const { return valorDesconto; }
double Reserva::getValorTotal() const { return valorTotal; }
double Reserva::getValorEntrada() const { return valorEntrada; }
StatusReserva Reserva::getStatus() const { return status; }

void Reserva::setStatus(StatusReserva novoStatus)
{
    status = novoStatus;
}

bool Reserva::ocupaData(const QDate& dia) const
{
    if (!dataCheckIn.isValid() || numDiarias <= 0) {
        return false;
    }
    // Check-out acontece no dia seguinte a ultima diaria ocupada
    return dia >= dataCheckIn && dia < getDataCheckOut();
}

bool Reserva::conflitaPeriodo(Unidade outraUnidade,
                              TipoQuarto outroQuarto,
                              const QDate& outroCheckIn,
                              int outrasDiarias) const
{
    if (unidade != outraUnidade || tipoQuarto != outroQuarto) {
        return false;
    }
    if (!dataCheckIn.isValid() || !outroCheckIn.isValid() || numDiarias <= 0 || outrasDiarias <= 0) {
        return false;
    }

    QDate outroCheckOut = outroCheckIn.addDays(outrasDiarias);
    // Dois intervalos [inicioA, fimA) e [inicioB, fimB) se cruzam quando inicioA < fimB e inicioB < fimA
    return outroCheckIn < getDataCheckOut() && dataCheckIn < outroCheckOut;
}

QString Reserva::gerarTextoResumo() const
{
    QLocale br(QLocale::Portuguese, QLocale::Brazil);

    QString texto;
    texto += QString("Reserva #%1\n").arg(id);
    texto += QString("Atendente: %1 (%2)\n").arg(nomeAtendente, usuarioAtendente);
    texto += QString("Cliente: %1\n").arg(cliente.getNome());
    texto += QString("CPF do cliente: %1\n").arg(cliente.getCpf());
    texto += QString("Localidade: %1\n").arg(InfoHotel::unidadeParaTexto(unidade));
    texto += QString("Tipo de quarto: %1 (diária %2)\n")
                 .arg(InfoHotel::tipoQuartoParaTexto(tipoQuarto))
                 .arg(br.toCurrencyString(valorDiaria, "R$ "));
    texto += QString("Check-in: %1 | Check-out: %2 (%3 diária(s))\n")
                 .arg(dataCheckIn.toString("dd/MM/yyyy"))
                 .arg(getDataCheckOut().toString("dd/MM/yyyy"))
                 .arg(numDiarias);
    texto += QString("Política de desconto: %1 (%2%)\n")
                 .arg(nomePoliticaDesconto)
                 .arg(percentualDesconto, 0, 'f', 0);
    texto += QString("Valor bruto: %1 | Desconto: %2\n")
                 .arg(br.toCurrencyString(valorBruto, "R$ "))
                 .arg(br.toCurrencyString(valorDesconto, "R$ "));
    texto += QString("Valor total: %1\n").arg(br.toCurrencyString(valorTotal, "R$ "));
    texto += QString("Valor da entrada (1/3): %1\n").arg(br.toCurrencyString(valorEntrada, "R$ "));
    texto += QString("Status: %1").arg(InfoHotel::statusParaTexto(status));

    return texto;
}
