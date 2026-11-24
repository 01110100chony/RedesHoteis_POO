#include "Cliente.h"

Cliente::Cliente(const QString& nome, const QString& cpf)
    : Pessoa(nome)
{
    setCpf(cpf);
}

QString Cliente::getCpf() const
{
    return formatarCpf(cpf);
}

QString Cliente::getCpfNumerico() const
{
    return cpf;
}

void Cliente::setCpf(const QString& novoCpf)
{
    QString limpo = limparCpf(novoCpf);
    cpf = limpo.isEmpty() ? novoCpf.trimmed() : limpo;
}

QString Cliente::getTipo() const
{
    return "Cliente";
}

QString Cliente::getResumoIdentificacao() const
{
    return QString("%1 - CPF: %2").arg(nome, getCpf());
}

QString Cliente::limparCpf(const QString& cpf)
{
    QString apenasDigitos;
    for (const QChar& c : cpf) {
        if (c.isDigit()) {
            apenasDigitos.append(c);
        }
    }
    return apenasDigitos;
}

QString Cliente::formatarCpf(const QString& cpf)
{
    QString digitos = limparCpf(cpf);
    if (digitos.size() != 11) {
        return cpf.trimmed();
    }
    return QString("%1.%2.%3-%4")
        .arg(digitos.mid(0, 3))
        .arg(digitos.mid(3, 3))
        .arg(digitos.mid(6, 3))
        .arg(digitos.mid(9, 2));
}

bool Cliente::validarCpf(const QString& cpf)
{
    QString digitos = limparCpf(cpf);
    if (digitos.size() != 11) {
        return false;
    }

    // Rejeita CPFs obvios com todos os digitos iguais (ex: 00000000000)
    bool todosIguais = true;
    for (int i = 1; i < digitos.size(); ++i) {
        if (digitos[i] != digitos[0]) {
            todosIguais = false;
            break;
        }
    }
    return !todosIguais;
}
