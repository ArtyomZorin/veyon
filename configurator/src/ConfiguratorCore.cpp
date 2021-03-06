/*
 * ConfiguratorCore.cpp - global instances for the Veyon Configurator
 *
 * Copyright (c) 2010-2018 Tobias Junghans <tobydox@users.sf.net>
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

#include <QFile>
#include <QFileInfo>

#include "ConfiguratorCore.h"
#include "CryptoCore.h"
#include "VeyonConfiguration.h"
#include "Filesystem.h"
#include "Logger.h"


bool ConfiguratorCore::createKeyPair( VeyonCore::UserRole role, const QString &destDir )
{
	QString privateKeyFileName = VeyonCore::filesystem().privateKeyPath( role, destDir );
	QString publicKeyFileName = VeyonCore::filesystem().publicKeyPath( role, destDir );

	VeyonCore::filesystem().ensurePathExists( QFileInfo( privateKeyFileName ).path() );
	VeyonCore::filesystem().ensurePathExists( QFileInfo( publicKeyFileName ).path() );

	qInfo() << "ConfiguratorCore: creating new key pair in" << privateKeyFileName << "and" << publicKeyFileName;

	CryptoCore::PrivateKey privateKey = CryptoCore::KeyGenerator().createRSA( CryptoCore::RsaKeySize );
	CryptoCore::PublicKey publicKey = privateKey.toPublicKey();

	if( privateKey.isNull() || publicKey.isNull() )
	{
		ilog_failed( "key generation" );
		return false;
	}

	QFile privateKeyFile( privateKeyFileName );
	QFile publicKeyFile( publicKeyFileName );

	if( privateKeyFile.exists() || publicKeyFile.exists() )
	{
		if( QMessageBox::question( nullptr, tr( "Overwrite keys" ),
								   tr( "Some of the key files are already existing. If you replace them "
									   "with newly generated ones you will have to update the public keys "
									   "on all computers as well. Do you want to continue?" ),
								   QMessageBox::Yes, QMessageBox::No ) == QMessageBox::No )
		{
			return false;
		}

		privateKeyFile.setPermissions( QFile::WriteOwner | QFile::WriteGroup | QFile::WriteOther );
		privateKeyFile.remove();

		publicKeyFile.setPermissions( QFile::WriteOwner | QFile::WriteGroup | QFile::WriteOther );
		publicKeyFile.remove();
	}

	if( privateKey.toPEMFile( privateKeyFileName ) == false )
	{
		ilog_failed( "saving private key" );
		return false;
	}

	if( publicKey.toPEMFile( publicKeyFileName ) == false )
	{
		ilog_failed( "saving public key" );
		return false;
	}

	privateKeyFile.setPermissions( QFile::ReadOwner | QFile::ReadUser | QFile::ReadGroup );
	publicKeyFile.setPermissions( QFile::ReadOwner | QFile::ReadUser | QFile::ReadGroup | QFile::ReadOther );

	printf( "...done, saved key-pair in\n\n%s\n\nand\n\n%s",
						privateKeyFileName.toUtf8().constData(),
						publicKeyFileName.toUtf8().constData() );
	printf( "\n\n\nFor now the file is only readable by "
				"root and members of group root (if you\n"
				"didn't ran this command as non-root).\n"
				"I suggest changing the ownership of the "
				"private key so that the file is\nreadable "
				"by all members of a special group to which "
				"all users belong who are\nallowed to use "
				"Veyon.\n\n\n" );
	return true;
}




bool ConfiguratorCore::importPublicKey( VeyonCore::UserRole role, const QString& publicKeyFile, const QString &destDir )
{
	// look whether the public key file is valid
	CryptoCore::PublicKey publicKey( publicKeyFile );
	if( publicKey.isNull() )
	{
		qCritical() << "ConfiguratorCore::importPublicKey(): file" << publicKeyFile
					<< "is not a valid public key file";
		return false;
	}

	QString destinationPublicKeyPath = VeyonCore::filesystem().publicKeyPath( role, destDir );

	VeyonCore::filesystem().ensurePathExists( QFileInfo( destinationPublicKeyPath ).path() );

	QFile destFile( destinationPublicKeyPath );
	if( destFile.exists() )
	{
		destFile.setPermissions( QFile::WriteOwner | QFile::WriteGroup | QFile::WriteOther );
		if( !destFile.remove() )
		{
			qCritical() << "ConfiguratorCore::importPublicKey(): could not remove "
							"existing public key file" << destFile.fileName();
			return false;
		}
	}

	// now try to copy it
	return publicKey.toPEMFile( destinationPublicKeyPath );
}
