#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H
#include <QLabel>
#include <QWidget>
#include <Qt>
#include "Game.h"

// Класс служит для того чтобы нажимать на клетки на поле и отправлять координаты нажатых клеток на сервер
class ClickableLabel : public QLabel {
    Q_OBJECT

public:
    explicit ClickableLabel(QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
    ~ClickableLabel();
    int x; // Координата x для метки
    int y; // Координата y для метки

signals:
    // Сигнал, испускаемый при нажатии на метку, передает координаты клика (x и y)
    void clicked(int x, int u);
private:
    Game *game; // Указатель на объект класса Game для взаимодействия с игровыми данными
protected:
    // Переопределение метода для обработки событий нажатия мыши
    void mousePressEvent(QMouseEvent* event);

};
#endif // CLICKABLELABEL_H
