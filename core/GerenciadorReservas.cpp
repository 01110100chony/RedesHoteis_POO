#include "GerenciadorReservas.h"

GerenciadorReservas::GerenciadorReservas()
{
    carregarAtendentesPadrao();
}

GerenciadorReservas& GerenciadorReservas::getInstance()
{
    static GerenciadorReservas instancia;
    return instancia;
}

void GerenciadorReservas::carregarAtendentesPadrao()
{
    atendentes.clear();
    atendentes.append(Atendente("Atendente 1", "atendente1", "senha1"));
    atendentes.append(Atendente("Atendente 2", "atendente2", "senha2"));
    atendentes.append(Atendente("Atendente 3", "atendente3", "senha3"));
    atendentes.append(Atendente("Atendente 4", "atendente4", "senha4"));
}

bool GerenciadorReservas::autenticar(const QString& usuario, const QString& senha)
{
    for (const Atendente& a : atendentes) {
        if (a.validarCredenciais(usuario, senha)) {
            atendenteLogado = &a;
            return true;
        }
    }
    return false;
}

void GerenciadorReservas::logout()
{
    atendenteLogado = nullptr;
}

bool GerenciadorReservas::temAtendenteLogado() const
{
    return atendenteLogado != nullptr;
}

const Atendente* GerenciadorReservas::getAtendenteLogado() const
{
    return atendenteLogado;
}

const QList<Cliente>& GerenciadorReservas::getClientes() const
{
    return clientes;
}

std::optional<Cliente> GerenciadorReservas::buscarClientePorCpf(const QString& cpf) const
{
    QString alvo = Cliente::limparCpf(cpf);
    if (alvo.isEmpty()) {
        return std::nullopt;
    }

    for (const Cliente& c : clientes) {
        if (c.getCpfNumerico() == alvo) {
            return c;
        }
    }
    return std::nullopt;
}

void GerenciadorReservas::cadastrarOuAtualizarCliente(const Cliente& cliente)
{
    QString cpfLimpo = cliente.getCpfNumerico();
    for (Cliente& existente : clientes) {
        if (existente.getCpfNumerico() == cpfLimpo) {
            existente.setNome(cliente.getNome());
            return;
        }
    }
    clientes.append(cliente);
}

bool GerenciadorReservas::periodoDentroDaJanela(const QDate& checkIn, int numDiarias, QString* motivo) const
{
    if (!checkIn.isValid()) {
        if (motivo) *motivo = "Data de check-in inválida.";
        return false;
    }
    if (numDiarias <= 0) {
        if (motivo) *motivo = "O número de diárias deve ser maior que zero.";
        return false;
    }

    QDate hoje = QDate::currentDate();
    if (checkIn < hoje) {
        if (motivo) *motivo = "A data de check-in não pode ser anterior à data de hoje.";
        return false;
    }

    // Mes corrente ate os proximos 12 meses (ultimo dia do 12o mes a frente)
    QDate mesLimite = hoje.addMonths(12);
    QDate dataLimite(mesLimite.year(), mesLimite.month(), mesLimite.daysInMonth());
    QDate ultimaDiaria = checkIn.addDays(numDiarias - 1);

    if (ultimaDiaria > dataLimite) {
        if (motivo) {
            *motivo = QString("Período fora da janela permitida (disponível até %1).")
                          .arg(dataLimite.toString("dd/MM/yyyy"));
        }
        return false;
    }

    return true;
}

ResultadoDisponibilidade GerenciadorReservas::consultarDisponibilidade(Unidade unidade,
                                                                       TipoQuarto quarto,
                                                                       const QDate& checkIn,
                                                                       int numDiarias) const
{
    ResultadoDisponibilidade res;

    QString motivoJanela;
    if (!periodoDentroDaJanela(checkIn, numDiarias, &motivoJanela)) {
        res.disponivel = false;
        res.mensagem = motivoJanela;
        return res;
    }

    int pendentes = 0;
    for (const Reserva& r : reservas) {
        if (r.getStatus() == StatusReserva::Cancelada) {
            continue;
        }
        if (!r.conflitaPeriodo(unidade, quarto, checkIn, numDiarias)) {
            continue;
        }

        if (r.getStatus() == StatusReserva::Confirmada) {
            res.disponivel = false;
            res.mensagem = QString("Quarto indisponível: já existe reserva confirmada (#%1) de %2 até %3.")
                               .arg(r.getId())
                               .arg(r.getDataCheckIn().toString("dd/MM/yyyy"))
                               .arg(r.getDataCheckOut().toString("dd/MM/yyyy"));
            return res;
        }

        if (r.getStatus() == StatusReserva::Pendente) {
            pendentes++;
        }
    }

    res.disponivel = true;
    res.possuiPendentes = (pendentes > 0);
    res.qtdPendentes = pendentes;

    if (pendentes > 0) {
        res.mensagem = QString("Quarto disponível (há %1 reserva(s) pendente(s) de pagamento nesse período; quem confirmar primeiro garante o quarto).")
                           .arg(pendentes);
    } else {
        res.mensagem = "Quarto disponível para todo o período solicitado.";
    }

    return res;
}
