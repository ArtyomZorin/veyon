/*
 * AuthKeysCommandLinePlugin.h - declaration of AuthKeysCommandLinePlugin class
 *
 * Copyright (c) 2018 Tobias Junghans <tobydox@users.sf.net>
 *
 * This file is part of Veyon - http://veyon.io
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program (see COPYING); if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 */

#ifndef AUTH_KEYS_COMMAND_LINE_PLUGIN_H
#define AUTH_KEYS_COMMAND_LINE_PLUGIN_H

#include "CommandLinePluginInterface.h"

class AuthKeysCommandLinePlugin : public QObject, CommandLinePluginInterface, PluginInterface
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "org.veyon.Veyon.Plugins.AuthKeysCommandLineInterface")
	Q_INTERFACES(PluginInterface CommandLinePluginInterface)
public:
	AuthKeysCommandLinePlugin( QObject* parent = nullptr );
	~AuthKeysCommandLinePlugin() override;

	Plugin::Uid uid() const override
	{
		return "4790bad8-4c56-40d5-8361-099a68f0c24b";
	}

	QVersionNumber version() const override
	{
		return QVersionNumber( 1, 0 );
	}

	QString name() const override
	{
		return QStringLiteral( "AuthKeys" );
	}

	QString description() const override
	{
		return tr( "Command line support for managing authentication keys" );
	}

	QString vendor() const override
	{
		return QStringLiteral( "Veyon Community" );
	}

	QString copyright() const override
	{
		return QStringLiteral( "Tobias Junghans" );
	}

	QString commandLineModuleName() const override
	{
		return QStringLiteral( "authkeys" );
	}

	QString commandLineModuleHelp() const override
	{
		return tr( "Commands for managing authentication keys" );
	}

	QStringList commands() const override;
	QString commandHelp( const QString& command ) const override;

public slots:
	CommandLinePluginInterface::RunResult handle_create( const QStringList& arguments );
	CommandLinePluginInterface::RunResult handle_list( const QStringList& arguments );

private:
	QMap<QString, QString> m_commands;

};

#endif // AUTH_KEYS_COMMAND_LINE_PLUGIN_H
