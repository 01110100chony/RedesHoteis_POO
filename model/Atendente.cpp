#include "Atendente.h"

Atendente::Atendente(const QString& nome, const QString& usuario, const QString& senha)
    : Pessoa(nome),
      usuario(usuario.trimmed()),
      senha(senha)
{
}

QString Atendente::getUsuario() const
{
    return usuario;
}

bool Atendente::validarCredenciais(const QString& user, const QString& pass) const
{
    return usuario == user.trimmed() && senha == pass;
}

QString Atendente::getTipo() const
{
    return "Atendente";
}

QString Atendente::getResumoIdentificacao() const
{
    return QString("%1 (%2)").arg(nome, usuario);
}
