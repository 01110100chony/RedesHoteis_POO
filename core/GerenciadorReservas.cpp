#include "GerenciadorReservas.h"
#include "PersistenciaArquivo.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <algorithm>

GerenciadorReservas::GerenciadorReservas()
{
    carregarAtendentesPadrao();

    if (QCoreApplication::instance()) {
        diretorioDados = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        if (diretorioDados.isEmpty()) {
            diretorioDados = QCoreApplication::applicationDirPath() + "/dados";
        } else {
            const QDir dadosAntigos(QDir(QCoreApplication::applicationDirPath()).filePath("dados"));
            const QDir dadosDoUsuario(diretorioDados);
            if (dadosAntigos.absolutePath() != dadosDoUsuario.absolutePath()) {
                for (const QString& nomeArquivo : {QStringLiteral("clientes.csv"), QStringLiteral("reservas.csv")}) {
                    const QString arquivoAntigo = dadosAntigos.filePath(nomeArquivo);
                    const QString arquivoNovo = dadosDoUsuario.filePath(nomeArquivo);
                    if (QFile::exists(arquivoAntigo) && !QFile::exists(arquivoNovo)) {
                        QDir().mkpath(diretorioDados);
                        QFile::copy(arquivoAntigo, arquivoNovo);
                    }
                }
            }
        }
    } else {
        diretorioDados = QDir::currentPath() + "/dados";
    }
    carregarDadosDoDisco();
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

void GerenciadorReservas::carregarDadosDoDisco()
{
    clientes = PersistenciaArquivo::carregarClientesCsv(getCaminhoArquivoClientes());
    reservas = PersistenciaArquivo::carregarReservasCsv(getCaminhoArquivoReservas());

    int maiorId = 0;
    for (const Reserva& r : reservas) {
        if (r.getId() > maiorId) {
            maiorId = r.getId();
        }
    }
    proximoId = maiorId + 1;
    ordenarReservasPorCheckIn();
}

void GerenciadorReservas::salvarClientesNoDisco() const
{
    PersistenciaArquivo::salvarClientesCsv(getCaminhoArquivoClientes(), clientes);
}

void GerenciadorReservas::salvarReservasNoDisco() const
{
    PersistenciaArquivo::salvarReservasCsv(getCaminhoArquivoReservas(), reservas);
}

void GerenciadorReservas::configurarDiretorioDados(const QString& novoDiretorio)
{
    diretorioDados = novoDiretorio;
    carregarDadosDoDisco();
}

void GerenciadorReservas::resetarParaTestes(const QString& diretorioTeste)
{
    diretorioDados = diretorioTeste;
    atendenteLogado = nullptr;
    clientes.clear();
    reservas.clear();
    proximoId = 1;

    QDir().mkpath(diretorioDados);
    QFile::remove(getCaminhoArquivoClientes());
    QFile::remove(getCaminhoArquivoReservas());
}

QString GerenciadorReservas::getCaminhoArquivoClientes() const
{
    return QDir(diretorioDados).filePath("clientes.csv");
}

QString GerenciadorReservas::getCaminhoArquivoReservas() const
{
    return QDir(diretorioDados).filePath("reservas.csv");
}

bool GerenciadorReservas::exportarReservas(const QString& caminhoArquivo) const
{
    if (caminhoArquivo.endsWith(".txt", Qt::CaseInsensitive)) {
        return PersistenciaArquivo::exportarReservasTxt(caminhoArquivo, reservas);
    }
    return PersistenciaArquivo::salvarReservasCsv(caminhoArquivo, reservas);
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
            salvarClientesNoDisco();
            return;
        }
    }
    clientes.append(cliente);
    salvarClientesNoDisco();
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

void GerenciadorReservas::ordenarReservasPorCheckIn()
{
    std::sort(reservas.begin(), reservas.end(), [](const Reserva& a, const Reserva& b) {
        if (a.getDataCheckIn() != b.getDataCheckIn()) {
            return a.getDataCheckIn() < b.getDataCheckIn();
        }
        return a.getId() < b.getId();
    });
}

QList<Reserva> GerenciadorReservas::cancelarPendentesConflitantes(int idIgnorar,
                                                                  Unidade unidade,
                                                                  TipoQuarto quarto,
                                                                  const QDate& checkIn,
                                                                  int numDiarias)
{
    QList<Reserva> canceladas;
    for (Reserva& r : reservas) {
        if (r.getId() == idIgnorar || r.getStatus() != StatusReserva::Pendente) {
            continue;
        }
        if (r.conflitaPeriodo(unidade, quarto, checkIn, numDiarias)) {
            r.setStatus(StatusReserva::Cancelada);
            canceladas.append(r);
        }
    }
    return canceladas;
}

CalculoReserva GerenciadorReservas::calcularValores(TipoQuarto quarto,
                                                    int numDiarias,
                                                    const EstrategiaDesconto& estrategia) const
{
    CalculoReserva calc;
    calc.valorDiaria = InfoHotel::valorDiaria(quarto);
    calc.valorBruto = calc.valorDiaria * numDiarias;
    calc.percentualDesconto = estrategia.getPercentual();
    calc.valorDesconto = estrategia.calcularDesconto(calc.valorBruto);
    calc.valorTotal = calc.valorBruto - calc.valorDesconto;
    calc.valorEntrada = calc.valorTotal / 3.0;
    calc.nomePolitica = estrategia.getNome();
    return calc;
}

ResultadoOperacaoReserva GerenciadorReservas::criarReserva(const QString& nomeCliente,
                                                           const QString& cpfCliente,
                                                           Unidade unidade,
                                                           TipoQuarto quarto,
                                                           const QDate& checkIn,
                                                           int numDiarias,
                                                           const EstrategiaDesconto& estrategia,
                                                           bool pagamentoConfirmado)
{
    ResultadoOperacaoReserva res;

    if (!temAtendenteLogado()) {
        res.mensagem = "Nenhum atendente autenticado no sistema.";
        return res;
    }

    QString nomeLimpo = nomeCliente.trimmed();
    if (nomeLimpo.isEmpty()) {
        res.mensagem = "Informe o nome completo do cliente.";
        return res;
    }

    if (!Cliente::validarCpf(cpfCliente)) {
        res.mensagem = "CPF inválido. Informe 11 dígitos numéricos válidos.";
        return res;
    }

    ResultadoDisponibilidade disp = consultarDisponibilidade(unidade, quarto, checkIn, numDiarias);
    if (!disp.disponivel) {
        res.mensagem = disp.mensagem;
        return res;
    }

    Cliente cliente(nomeLimpo, cpfCliente);
    cadastrarOuAtualizarCliente(cliente);

    CalculoReserva calc = calcularValores(quarto, numDiarias, estrategia);
    StatusReserva statusInicial = pagamentoConfirmado ? StatusReserva::Confirmada : StatusReserva::Pendente;

    int novoId = proximoId++;
    Reserva nova(novoId,
                 atendenteLogado->getUsuario(),
                 atendenteLogado->getNome(),
                 cliente,
                 unidade,
                 quarto,
                 checkIn,
                 numDiarias,
                 calc.nomePolitica,
                 calc.percentualDesconto,
                 calc.valorDiaria,
                 calc.valorBruto,
                 calc.valorDesconto,
                 calc.valorTotal,
                 calc.valorEntrada,
                 statusInicial);

    reservas.append(nova);

    if (pagamentoConfirmado) {
        res.reservasCanceladas = cancelarPendentesConflitantes(novoId, unidade, quarto, checkIn, numDiarias);
    }

    ordenarReservasPorCheckIn();
    salvarReservasNoDisco();

    res.sucesso = true;
    res.idReserva = novoId;
    res.mensagem = pagamentoConfirmado
                       ? QString("Reserva #%1 registrada e confirmada com sucesso.").arg(novoId)
                       : QString("Reserva #%1 registrada como pendente de confirmação.").arg(novoId);
    return res;
}

ResultadoOperacaoReserva GerenciadorReservas::confirmarPagamento(int idReserva)
{
    ResultadoOperacaoReserva res;

    Reserva* alvo = nullptr;
    for (Reserva& r : reservas) {
        if (r.getId() == idReserva) {
            alvo = &r;
            break;
        }
    }

    if (!alvo) {
        res.mensagem = "Reserva não encontrada.";
        return res;
    }

    if (alvo->getStatus() == StatusReserva::Confirmada) {
        res.mensagem = "Esta reserva já está confirmada.";
        return res;
    }
    if (alvo->getStatus() == StatusReserva::Cancelada) {
        res.mensagem = "Não é possível confirmar uma reserva cancelada.";
        return res;
    }

    // Garante que nenhuma outra reserva confirmada ocupa o mesmo periodo
    for (const Reserva& outra : reservas) {
        if (outra.getId() == alvo->getId() || outra.getStatus() != StatusReserva::Confirmada) {
            continue;
        }
        if (outra.conflitaPeriodo(alvo->getUnidade(),
                                  alvo->getTipoQuarto(),
                                  alvo->getDataCheckIn(),
                                  alvo->getNumDiarias())) {
            res.mensagem = QString("Conflito: a reserva confirmada #%1 já ocupa esse quarto no período.")
                               .arg(outra.getId());
            return res;
        }
    }

    alvo->setStatus(StatusReserva::Confirmada);
    res.reservasCanceladas = cancelarPendentesConflitantes(alvo->getId(),
                                                           alvo->getUnidade(),
                                                           alvo->getTipoQuarto(),
                                                           alvo->getDataCheckIn(),
                                                           alvo->getNumDiarias());
    salvarReservasNoDisco();

    res.sucesso = true;
    res.idReserva = alvo->getId();
    res.mensagem = QString("Pagamento da reserva #%1 confirmado!").arg(alvo->getId());
    return res;
}

ResultadoOperacaoReserva GerenciadorReservas::cancelarReserva(int idReserva)
{
    ResultadoOperacaoReserva res;

    for (Reserva& r : reservas) {
        if (r.getId() == idReserva) {
            if (r.getStatus() == StatusReserva::Cancelada) {
                res.mensagem = "Esta reserva já está cancelada.";
                return res;
            }
            r.setStatus(StatusReserva::Cancelada);
            salvarReservasNoDisco();
            res.sucesso = true;
            res.idReserva = idReserva;
            res.mensagem = QString("Reserva #%1 cancelada.").arg(idReserva);
            return res;
        }
    }

    res.mensagem = "Reserva não encontrada.";
    return res;
}

QList<Reserva> GerenciadorReservas::listarReservasOrdenadas() const
{
    return reservas;
}

std::optional<Reserva> GerenciadorReservas::buscarReservaPorId(int idReserva) const
{
    for (const Reserva& r : reservas) {
        if (r.getId() == idReserva) {
            return r;
        }
    }
    return std::nullopt;
}
