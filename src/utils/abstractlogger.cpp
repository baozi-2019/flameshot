#include "abstractlogger.h"
#include "utils/systemnotification.h"

#include <QFileInfo>

namespace {
// Test a Target bitmask without signed bitwise operands and with an explicit
// comparison instead of an implicit int-to-bool conversion.
bool hasTarget(int targets, AbstractLogger::Target target)
{
    return (static_cast<unsigned int>(targets) &
            static_cast<unsigned int>(target)) != 0;
}
} // unnamed namespace

AbstractLogger::AbstractLogger(Channel channel, int targets)
  : m_defaultChannel(channel)
  , m_targets(targets)
{
    if (hasTarget(targets, LogFile)) {
        // TODO
    }
}

/**
 * @brief Construct an AbstractLogger with output to a string.
 * @param additionalChannels Optional additional targets to output to.
 */
AbstractLogger::AbstractLogger(QString& str,
                               Channel channel,
                               int additionalTargets)
  : AbstractLogger(channel, additionalTargets)
{
    m_textStreams << new QTextStream(&str);
}

AbstractLogger::~AbstractLogger()
{
    qDeleteAll(m_textStreams);
}

AbstractLogger AbstractLogger::info(int targets)
{
    return AbstractLogger(Info, targets);
}

AbstractLogger AbstractLogger::warning(int targets)
{
    return AbstractLogger(Warning, targets);
}

AbstractLogger AbstractLogger::error(int targets)
{
    return AbstractLogger(Error, targets);
}

AbstractLogger& AbstractLogger::sendMessage(const QString& msg, Channel channel)
{
    if (hasTarget(m_targets, Notification)) {
        SystemNotification().sendMessage(
          msg, messageHeader(channel, Notification), m_notificationPath);
    }
    if (!m_textStreams.isEmpty()) {
        for (auto* stream : m_textStreams) { // NOLINT(altera-unroll-loops)
            *stream << messageHeader(channel, String) << msg << "\n";
        }
    }
    if (hasTarget(m_targets, LogFile)) {
        // TODO
    }
    if (hasTarget(m_targets, Stderr)) {
        QTextStream stream(stderr);
        stream << messageHeader(channel, Stderr) << msg << "\n";
    }

    if (hasTarget(m_targets, Stdout)) {
        QTextStream stream(stdout);
        stream << messageHeader(channel, Stdout) << msg << "\n";
    }

    return *this;
}

/**
 * @brief Send a message to the default channel of this logger.
 * @param msg
 * @return
 */
AbstractLogger& AbstractLogger::operator<<(const QString& msg)
{
    sendMessage(msg, m_defaultChannel);
    return *this;
}

AbstractLogger& AbstractLogger::addOutputString(QString& str)
{
    m_textStreams << new QTextStream(&str);
    return *this;
}

/**
 * @brief Attach a path to a notification so it can be dragged and dropped.
 */
AbstractLogger& AbstractLogger::attachNotificationPath(const QString& path)
{
    if (hasTarget(m_targets, Notification)) {
        m_notificationPath = path;
    }
    // Attaching a path without a notification target is silently ignored.
    return *this;
}

/**
 * @brief Enable/disable message header (e.g. "flameshot: info:").
 */
AbstractLogger& AbstractLogger::enableMessageHeader(bool enable)
{
    m_enableMessageHeader = enable;
    return *this;
}

/**
 * @brief Generate a message header for the given channel and target.
 */
QString AbstractLogger::messageHeader(Channel channel, Target target) const
{
    if (!m_enableMessageHeader) {
        return "";
    }
    QString messageChannel;
    if (channel == Info) {
        messageChannel = "info";
    } else if (channel == Warning) {
        messageChannel = "warning";
    } else if (channel == Error) {
        messageChannel = "error";
    }

    if (target == Notification) {
        messageChannel[0] = messageChannel[0].toUpper();
        return "Flameshot " + messageChannel;
    }
    return "flameshot: " + messageChannel + ": ";
}
