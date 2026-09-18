// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2017-2019 Alejandro Sirgo Rica & Contributors

#pragma once

#include <QPoint>
#include <QTextEdit>

class QEvent;
class QKeyEvent;
class QMouseEvent;

class TextWidget : public QTextEdit
{
    Q_OBJECT
public:
    explicit TextWidget(QWidget* parent = nullptr);

    void adjustSize();
    void setFont(const QFont& f);

protected:
    bool event(QEvent* e) override;
    void keyPressEvent(QKeyEvent* e) override;
    void showEvent(QShowEvent* e) override;
    void resizeEvent(QResizeEvent* e) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

signals:
    void textUpdated(const QString& s);
    void editingFinished();
    // Emitted while the widget is dragged by its margin; the argument is
    // the new top-left position in the parent (CaptureWidget) coordinates.
    void dragged(const QPoint& newTopLeft);

public slots:
    void setTextColor(const QColor& c);
    void setAlignment(Qt::AlignmentFlag alignment);

private:
    void emitTextUpdated();
    [[nodiscard]] bool isOnDragMargin(const QPoint& pos) const;

    QSize m_baseSize;
    QSize m_minSize;
    bool m_dragging{ false };
    QPoint m_dragOffset;
};
