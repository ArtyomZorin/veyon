/*
 * Filesystem.cpp - filesystem related query and manipulation functions
 *
 * Copyright (c) 2017-2018 Tobias Junghans <tobydox@users.sf.net>
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

#include <veyonconfig.h>

#include <QCoreApplication>
#include <QDir>

#include "VeyonConfiguration.h"
#include "Filesystem.h"
#include "PlatformCoreFunctions.h"


QString Filesystem::expandPath( QString path ) const
{
	const auto p = QDir::toNativeSeparators( path.replace( QStringLiteral( "%HOME%" ), QDir::homePath() ).
											 replace( QStringLiteral( "$HOME" ), QDir::homePath() ).
											 replace( QStringLiteral( "%PROFILE%" ), QDir::homePath() ).
											 replace( QStringLiteral( "$PROFILE" ), QDir::homePath() ).
											 replace( QStringLiteral( "%APPDATA%" ), VeyonCore::platform().coreFunctions().personalAppDataPath() ).
											 replace( QStringLiteral( "$APPDATA" ), VeyonCore::platform().coreFunctions().personalAppDataPath() ).
											 replace( QStringLiteral( "%GLOBALAPPDATA%" ), VeyonCore::platform().coreFunctions().globalAppDataPath() ).
											 replace( QStringLiteral( "$GLOBALAPPDATA" ), VeyonCore::platform().coreFunctions().globalAppDataPath() ).
											 replace( QStringLiteral( "%TMP%" ), QDir::tempPath() ).
											 replace( QStringLiteral( "$TMP" ), QDir::tempPath() ).
											 replace( QStringLiteral( "%TEMP%" ), QDir::tempPath() ).
											 replace( QStringLiteral( "$TEMP" ), QDir::tempPath() ) );

	// remove duplicate directory separators - however skip the first two chars
	// as they might specify an UNC path on Windows
	if( p.length() > 3 )
	{
		return p.left( 2 ) + p.mid( 2 ).replace(
					QString( QStringLiteral( "%1%1" ) ).arg( QDir::separator() ),
					QDir::separator() );
	}

	return p;
}



QString Filesystem::shrinkPath( QString path ) const
{
	if( QFileInfo( path ).isDir() )
	{
		// we replace parts of the path with strings returned by
		// personalConfigDataPath() & friends which always return a path with
		// a trailing dir separator - therefore add one so we don't miss a
		// replace
		path += QDir::separator();
	}

	path = QDir::toNativeSeparators( path );

	const QString envVar( QStringLiteral( "%%1%" ) + QDir::separator() );
	const auto personalAppDataPath = VeyonCore::platform().coreFunctions().personalAppDataPath();
	const auto globalAppDataPath = VeyonCore::platform().coreFunctions().globalAppDataPath();

	if( path.startsWith( personalAppDataPath ) )
	{
		path.replace( personalAppDataPath, envVar.arg( QStringLiteral( "APPDATA" ) ) );
	}
	else if( path.startsWith( globalAppDataPath ) )
	{
		path.replace( globalAppDataPath, envVar.arg( QStringLiteral( "GLOBALAPPDATA" ) ) );
	}
	else if( path.startsWith( QDir::toNativeSeparators( QDir::homePath() ) ) )
	{
		path.replace( QDir::toNativeSeparators( QDir::homePath() ), envVar.arg( QStringLiteral( "HOME" ) ) );
	}
	else if( path.startsWith( QDir::toNativeSeparators( QDir::tempPath() ) ) )
	{
		path.replace( QDir::toNativeSeparators( QDir::tempPath() ), envVar.arg( QStringLiteral( "TEMP" ) ) );
	}

	// remove duplicate directory separators - however skip the first two chars
	// as they might specify an UNC path on Windows
	if( path.length() > 3 )
	{
		return QDir::toNativeSeparators( path.left( 2 ) + path.mid( 2 ).replace(
											 QString( QStringLiteral( "%1%1" ) ).arg( QDir::separator() ), QDir::separator() ) );
	}

	return QDir::toNativeSeparators( path );
}




bool Filesystem::ensurePathExists( const QString &path ) const
{
	const QString expandedPath = VeyonCore::filesystem().expandPath( path );

	if( path.isEmpty() || QDir( expandedPath ).exists() )
	{
		return true;
	}

	qDebug() << "Filesystem::ensurePathExists(): creating " << path << "=>" << expandedPath;

	QString p = expandedPath;

	QStringList dirs;
	while( !QDir( p ).exists() && !p.isEmpty() )
	{
		dirs.push_front( QDir( p ).dirName() );
		p.chop( dirs.front().size() + 1 );
	}

	if( !p.isEmpty() )
	{
		return QDir( p ).mkpath( dirs.join( QDir::separator() ) );
	}

	return false;
}



QString Filesystem::privateKeyPath( const QString& name ) const
{
	const auto d = VeyonCore::filesystem().expandPath( VeyonCore::config().privateKeyBaseDir() ) +
			QDir::separator() + name + QDir::separator() + QStringLiteral( "key" );

	return QDir::toNativeSeparators( d );
}



QString Filesystem::publicKeyPath( const QString& name ) const
{
	const auto d = VeyonCore::filesystem().expandPath( VeyonCore::config().publicKeyBaseDir() ) +
			QDir::separator() + name + QDir::separator() + QStringLiteral( "key" );

	return QDir::toNativeSeparators( d );
}



QString Filesystem::privateKeyPath( VeyonCore::UserRoles role, QString baseDir ) const
{
	if( baseDir.isEmpty() )
	{
		baseDir = VeyonCore::filesystem().expandPath( VeyonCore::config().privateKeyBaseDir() );
	}
	else
	{
		baseDir += QDir::separator() + QStringLiteral( "private" );
	}

	const auto d = baseDir + QDir::separator() + VeyonCore::userRoleName( role ) + QDir::separator() + QStringLiteral( "key" );

	return QDir::toNativeSeparators( d );
}



QString Filesystem::publicKeyPath( VeyonCore::UserRoles role, QString baseDir ) const
{
	if( baseDir.isEmpty() )
	{
		baseDir = VeyonCore::filesystem().expandPath( VeyonCore::config().publicKeyBaseDir() );
	}
	else
	{
		baseDir += QDir::separator() + QStringLiteral( "public" );
	}

	const auto d = baseDir + QDir::separator() + VeyonCore::userRoleName( role ) + QDir::separator() + QStringLiteral( "key" );

	return QDir::toNativeSeparators( d );
}



QString Filesystem::serverFilePath() const
{
	return QDir::toNativeSeparators( QCoreApplication::applicationDirPath() + QDir::separator() +
									 QStringLiteral("veyon-server" VEYON_EXECUTABLE_SUFFIX) );
}



QString Filesystem::workerFilePath() const
{
	return QDir::toNativeSeparators( QCoreApplication::applicationDirPath() + QDir::separator() +
									 QStringLiteral("veyon-worker" VEYON_EXECUTABLE_SUFFIX) );
}
