#pragma once

#include <QList>
#include <QString>
#include <QTextStream>

#include <cstdint>

/**
 * @brief A class that allows you to log events to where they need to go.
 */
class AbstractLogger
{
public:
    enum Target : std::uint8_t
    {
        Notification = 0x01,
        Stderr = 0x02,
        LogFile = 0x08,
        String = 0x10,
        Stdout = 0x20,
        // Notification | LogFile | Stderr; written as a literal because the
        // bitwise OR of the enumerators promotes to signed int, which
        // hicpp-signed-bitwise rejects even with an unsigned base type.
        Default = 0x2B,
    };

    enum Channel : std::uint8_t
    {
        Info,
        Warning,
        Error
    };

    explicit AbstractLogger(Channel channel = Info, int targets = Default);
    explicit AbstractLogger(QString& str,
                            Channel channel,
                            int additionalTargets = String);
    ~AbstractLogger();

    // The logger owns its output streams; copying would double-delete them.
    AbstractLogger(const AbstractLogger&) = delete;
    AbstractLogger& operator=(const AbstractLogger&) = delete;
    AbstractLogger(AbstractLogger&&) = default;
    AbstractLogger& operator=(AbstractLogger&&) = default;

    // Convenience functions
    static AbstractLogger info(int targets = Default);
    static AbstractLogger warning(int targets = Default);
    static AbstractLogger error(int targets = Default);

    AbstractLogger& sendMessage(const QString& msg, Channel channel);
    AbstractLogger& operator<<(const QString& msg);
    AbstractLogger& addOutputString(QString& str);
    AbstractLogger& attachNotificationPath(const QString& path);
    AbstractLogger& enableMessageHeader(bool enable);

private:
    [[nodiscard]] QString messageHeader(Channel channel, Target target) const;

    int m_targets;
    Channel m_defaultChannel;
    QList<QTextStream*> m_textStreams;
    QString m_notificationPath;
    bool m_enableMessageHeader = true;
};
