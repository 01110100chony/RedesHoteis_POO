#ifndef TELARESUMORESERVA_H
#define TELARESUMORESERVA_H

#include "model/Reserva.h"
#include <QDialog>

class TelaResumoReserva : public QDialog
{
    Q_OBJECT

public:
    explicit TelaResumoReserva(const Reserva& reserva, QWidget* parent = nullptr);
};

#endif // TELARESUMORESERVA_H
