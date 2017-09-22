/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -i remote_adaptor.h -a :remote_adaptor.cpp remote.xml
 *
 * qdbusxml2cpp is Copyright (C) 2015 Digia Plc and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#include "remote_adaptor.h"
#include <QtCore/QMetaObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

/*
 * Implementation of adaptor class RemoteAdaptor
 */

RemoteAdaptor::RemoteAdaptor(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    // constructor
    setAutoRelaySignals(true);
}

RemoteAdaptor::~RemoteAdaptor()
{
    // destructor
}

bool RemoteAdaptor::changeSyncSrcMode(int mode, int channel)
{
    // handle method call com.inno.f280.remote.changeSyncSrcMode
    bool out0;
    QMetaObject::invokeMethod(parent(), "changeSyncSrcMode", Q_RETURN_ARG(bool, out0), Q_ARG(int, mode), Q_ARG(int, channel));
    return out0;
}

void RemoteAdaptor::emulateGesture()
{
    // handle method call com.inno.f280.remote.emulateGesture
    QMetaObject::invokeMethod(parent(), "emulateGesture");
}

void RemoteAdaptor::emulateKey(int type, int key, int modifier)
{
    // handle method call com.inno.f280.remote.emulateKey
    QMetaObject::invokeMethod(parent(), "emulateKey", Q_ARG(int, type), Q_ARG(int, key), Q_ARG(int, modifier));
}

void RemoteAdaptor::emulateMouse(int type, int x, int y, int button)
{
    // handle method call com.inno.f280.remote.emulateMouse
    QMetaObject::invokeMethod(parent(), "emulateMouse", Q_ARG(int, type), Q_ARG(int, x), Q_ARG(int, y), Q_ARG(int, button));
}

void RemoteAdaptor::emulateTouch()
{
    // handle method call com.inno.f280.remote.emulateTouch
    QMetaObject::invokeMethod(parent(), "emulateTouch");
}

bool RemoteAdaptor::loadStorageInfo(int index)
{
    // handle method call com.inno.f280.remote.loadStorageInfo
    bool out0;
    QMetaObject::invokeMethod(parent(), "loadStorageInfo", Q_RETURN_ARG(bool, out0), Q_ARG(int, index));
    return out0;
}

QString RemoteAdaptor::readField(const QString &path, const QString &role, int row, int col)
{
    // handle method call com.inno.f280.remote.readField
    QString out0;
    QMetaObject::invokeMethod(parent(), "readField", Q_RETURN_ARG(QString, out0), Q_ARG(QString, path), Q_ARG(QString, role), Q_ARG(int, row), Q_ARG(int, col));
    return out0;
}

bool RemoteAdaptor::removeStorageInfo(int index)
{
    // handle method call com.inno.f280.remote.removeStorageInfo
    bool out0;
    QMetaObject::invokeMethod(parent(), "removeStorageInfo", Q_RETURN_ARG(bool, out0), Q_ARG(int, index));
    return out0;
}

bool RemoteAdaptor::renameStorageFile(int index, const QString &name)
{
    // handle method call com.inno.f280.remote.renameStorageFile
    bool out0;
    QMetaObject::invokeMethod(parent(), "renameStorageFile", Q_RETURN_ARG(bool, out0), Q_ARG(int, index), Q_ARG(QString, name));
    return out0;
}

bool RemoteAdaptor::requestAnalyzerRestart(const QString &activityName)
{
    // handle method call com.inno.f280.remote.requestAnalyzerRestart
    bool out0;
    QMetaObject::invokeMethod(parent(), "requestAnalyzerRestart", Q_RETURN_ARG(bool, out0), Q_ARG(QString, activityName));
    return out0;
}

bool RemoteAdaptor::requestAnalyzerStart(const QString &activityName, const QString &time)
{
    // handle method call com.inno.f280.remote.requestAnalyzerStart
    bool out0;
    QMetaObject::invokeMethod(parent(), "requestAnalyzerStart", Q_RETURN_ARG(bool, out0), Q_ARG(QString, activityName), Q_ARG(QString, time));
    return out0;
}

bool RemoteAdaptor::requestAnalyzerStop()
{
    // handle method call com.inno.f280.remote.requestAnalyzerStop
    bool out0;
    QMetaObject::invokeMethod(parent(), "requestAnalyzerStop", Q_RETURN_ARG(bool, out0));
    return out0;
}

QString RemoteAdaptor::requestDeviceID()
{
    // handle method call com.inno.f280.remote.requestDeviceID
    QString out0;
    QMetaObject::invokeMethod(parent(), "requestDeviceID", Q_RETURN_ARG(QString, out0));
    return out0;
}

bool RemoteAdaptor::requestFirmwareUpgrade()
{
    // handle method call com.inno.f280.remote.requestFirmwareUpgrade
    bool out0;
    QMetaObject::invokeMethod(parent(), "requestFirmwareUpgrade", Q_RETURN_ARG(bool, out0));
    return out0;
}

QString RemoteAdaptor::requestFirmwareVersion()
{
    // handle method call com.inno.f280.remote.requestFirmwareVersion
    QString out0;
    QMetaObject::invokeMethod(parent(), "requestFirmwareVersion", Q_RETURN_ARG(QString, out0));
    return out0;
}

bool RemoteAdaptor::requestGPSStatus()
{
    // handle method call com.inno.f280.remote.requestGPSStatus
    bool out0;
    QMetaObject::invokeMethod(parent(), "requestGPSStatus", Q_RETURN_ARG(bool, out0));
    return out0;
}

bool RemoteAdaptor::requestSetSystemTime(const QString &time)
{
    // handle method call com.inno.f280.remote.requestSetSystemTime
    bool out0;
    QMetaObject::invokeMethod(parent(), "requestSetSystemTime", Q_RETURN_ARG(bool, out0), Q_ARG(QString, time));
    return out0;
}

bool RemoteAdaptor::requestSystemReboot()
{
    // handle method call com.inno.f280.remote.requestSystemReboot
    bool out0;
    QMetaObject::invokeMethod(parent(), "requestSystemReboot", Q_RETURN_ARG(bool, out0));
    return out0;
}

bool RemoteAdaptor::requestSystemRootLogin(const QString &code)
{
    // handle method call com.inno.f280.remote.requestSystemRootLogin
    bool out0;
    QMetaObject::invokeMethod(parent(), "requestSystemRootLogin", Q_RETURN_ARG(bool, out0), Q_ARG(QString, code));
    return out0;
}

bool RemoteAdaptor::requestSystemRootLogout()
{
    // handle method call com.inno.f280.remote.requestSystemRootLogout
    bool out0;
    QMetaObject::invokeMethod(parent(), "requestSystemRootLogout", Q_RETURN_ARG(bool, out0));
    return out0;
}

bool RemoteAdaptor::requestSystemShutdown()
{
    // handle method call com.inno.f280.remote.requestSystemShutdown
    bool out0;
    QMetaObject::invokeMethod(parent(), "requestSystemShutdown", Q_RETURN_ARG(bool, out0));
    return out0;
}

QDBusVariant RemoteAdaptor::requestSystemSnapshot()
{
    // handle method call com.inno.f280.remote.requestSystemSnapshot
    QDBusVariant out0;
    QMetaObject::invokeMethod(parent(), "requestSystemSnapshot", Q_RETURN_ARG(QDBusVariant, out0));
    return out0;
}

QString RemoteAdaptor::requestSystemTime()
{
    // handle method call com.inno.f280.remote.requestSystemTime
    QString out0;
    QMetaObject::invokeMethod(parent(), "requestSystemTime", Q_RETURN_ARG(QString, out0));
    return out0;
}

bool RemoteAdaptor::saveStorageInfo(int index)
{
    // handle method call com.inno.f280.remote.saveStorageInfo
    bool out0;
    QMetaObject::invokeMethod(parent(), "saveStorageInfo", Q_RETURN_ARG(bool, out0), Q_ARG(int, index));
    return out0;
}

QString RemoteAdaptor::storageFileList()
{
    // handle method call com.inno.f280.remote.storageFileList
    QString out0;
    QMetaObject::invokeMethod(parent(), "storageFileList", Q_RETURN_ARG(QString, out0));
    return out0;
}

int RemoteAdaptor::storageItemCount()
{
    // handle method call com.inno.f280.remote.storageItemCount
    int out0;
    QMetaObject::invokeMethod(parent(), "storageItemCount", Q_RETURN_ARG(int, out0));
    return out0;
}

QString RemoteAdaptor::storageItemInfo(int index)
{
    // handle method call com.inno.f280.remote.storageItemInfo
    QString out0;
    QMetaObject::invokeMethod(parent(), "storageItemInfo", Q_RETURN_ARG(QString, out0), Q_ARG(int, index));
    return out0;
}

bool RemoteAdaptor::writeField(const QString &path, const QString &role, const QDBusVariant &data, int row, int col)
{
    // handle method call com.inno.f280.remote.writeField
    bool out0;
    QMetaObject::invokeMethod(parent(), "writeField", Q_RETURN_ARG(bool, out0), Q_ARG(QString, path), Q_ARG(QString, role), Q_ARG(QDBusVariant, data), Q_ARG(int, row), Q_ARG(int, col));
    return out0;
}

