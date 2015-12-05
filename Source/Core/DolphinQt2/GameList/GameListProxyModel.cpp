// Copyright 2015 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the license.txt file included.

#include "DolphinQt2/Resources.h"
#include "DolphinQt2/GameList/GameListModel.h"
#include "DolphinQt2/GameList/GameListProxyModel.h"

static constexpr QSize NORMAL_BANNER_SIZE(96, 32);
static constexpr QSize LARGE_BANNER_SIZE(144, 48);

// Convert an integer size to a friendly string representation.
static QString FormatSize(qint64 size)
{
	QStringList units{
		QStringLiteral("KB"),
		QStringLiteral("MB"),
		QStringLiteral("GB"),
		QStringLiteral("TB")
	};
	QStringListIterator i(units);
	QString unit = QStringLiteral("B");
	double num = (double) size;
	while (num > 1024.0 && i.hasNext())
	{
		unit = i.next();
		num /= 1024.0;
	}
	return QStringLiteral("%1 %2").arg(QString::number(num, 'f', 1)).arg(unit);
}

GameListProxyModel::GameListProxyModel(QObject* parent)
	: QSortFilterProxyModel(parent)
{
}

QVariant GameListProxyModel::data(const QModelIndex& i, int role) const
{
	QModelIndex source_index = mapToSource(i);
	QVariant source_data = sourceModel()->data(source_index, Qt::DisplayRole);
	if (role == Qt::DisplayRole)
	{
		switch (i.column())
		{
		// Sort by the integer but display the formatted string.
		case GameListModel::COL_SIZE:
			return FormatSize(source_data.toULongLong());
		// These fall through to the underlying model.
		case GameListModel::COL_ID:
		case GameListModel::COL_TITLE:
		case GameListModel::COL_DESCRIPTION:
		case GameListModel::COL_MAKER:
			return source_data;
		// Show the title in the display role of the icon view.
		case GameListModel::COL_LARGE_ICON:
			return data(index(i.row(), GameListModel::COL_TITLE), Qt::DisplayRole);
		}
	}
	else if (role == Qt::DecorationRole)
	{
		switch (i.column())
		{
		// Show icons in the decoration roles. This lets us sort by the
		// underlying ints, but display just the icons without doing any
		// fixed-width hacks.
		case GameListModel::COL_PLATFORM:
			return Resources::GetPlatform(source_data.toInt());
		case GameListModel::COL_BANNER:
			return source_data.value<QPixmap>().scaled(
					NORMAL_BANNER_SIZE,
					Qt::KeepAspectRatio,
					Qt::SmoothTransformation);
		case GameListModel::COL_COUNTRY:
			return Resources::GetCountry(source_data.toInt());
		case GameListModel::COL_RATING:
			return Resources::GetRating(source_data.toInt());
		// Show a scaled icon in the decoration role of the icon view.
		case GameListModel::COL_LARGE_ICON:
			return data(index(i.row(), GameListModel::COL_BANNER), Qt::DecorationRole)
				.value<QPixmap>().scaled(
					LARGE_BANNER_SIZE,
					Qt::KeepAspectRatio,
					Qt::SmoothTransformation);
		}
	}
	return QVariant();
}
