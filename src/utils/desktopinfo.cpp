// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2017-2019 Alejandro Sirgo Rica & Contributors

#include "desktopinfo.h"

#include <QProcessEnvironment>

DesktopInfo::DesktopInfo()
{
    auto environment = QProcessEnvironment::systemEnvironment();
    XDG_CURRENT_DESKTOP = environment.value(QStringLiteral("XDG_CURRENT_DESKTOP"));
    XDG_SESSION_TYPE = environment.value(QStringLiteral("XDG_SESSION_TYPE"));
    WAYLAND_DISPLAY = environment.value(QStringLiteral("WAYLAND_DISPLAY"));
    KDE_FULL_SESSION = environment.value(QStringLiteral("KDE_FULL_SESSION"));
    GNOME_DESKTOP_SESSION_ID =
      environment.value(QStringLiteral("GNOME_DESKTOP_SESSION_ID"));
    DESKTOP_SESSION = environment.value(QStringLiteral("DESKTOP_SESSION"));
}

bool DesktopInfo::waylandDetected()
{
    return XDG_SESSION_TYPE == QLatin1String("wayland") ||
           WAYLAND_DISPLAY.contains(QLatin1String("wayland"),
                                    Qt::CaseInsensitive);
}

DesktopInfo::WM DesktopInfo::windowManager()
{
    DesktopInfo::WM res = DesktopInfo::OTHER;
    QStringList desktops = XDG_CURRENT_DESKTOP.split(QChar(':'));
    for (auto& desktop : desktops) { // NOLINT(altera-unroll-loops)
        if (desktop.contains(QLatin1String("GNOME"), Qt::CaseInsensitive)) {
            return DesktopInfo::GNOME;
        }
        if (desktop.contains(QLatin1String("qtile"), Qt::CaseInsensitive)) {
            return DesktopInfo::QTILE;
        }
        if (desktop.contains(QLatin1String("sway"), Qt::CaseInsensitive) ||
            desktop.contains(QLatin1String("river"), Qt::CaseInsensitive)) {
            return DesktopInfo::WLROOTS;
        }
        if (desktop.contains(QLatin1String("Hyprland"), Qt::CaseInsensitive)) {
            return DesktopInfo::HYPRLAND;
        }
        if (desktop.contains(QLatin1String("kde-plasma"))) {
            return DesktopInfo::KDE;
        }
        if (desktop.contains(QLatin1String("cosmic"), Qt::CaseInsensitive)) {
            return DesktopInfo::COSMIC;
        }
    }

    if (!GNOME_DESKTOP_SESSION_ID.isEmpty()) {
        return DesktopInfo::GNOME;
    }

    if (!KDE_FULL_SESSION.isEmpty()) {
        return DesktopInfo::KDE;
    }

    return res;
}
