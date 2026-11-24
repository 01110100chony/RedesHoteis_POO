#include "Pessoa.h"

Pessoa::Pessoa(const QString& nome)
    : nome(nome.trimmed())
{
}

QString Pessoa::getNome() const
{
    return nome;
}

void Pessoa::setNome(const QString& novoNome)
{
    nome = novoNome.trimmed();
}

QString Pessoa::getResumoIdentificacao() const
{
    return nome;
}
