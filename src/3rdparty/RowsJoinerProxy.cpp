/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * yshurik@lynxline.com wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Oleksandr Iakovliev
 * ----------------------------------------------------------------------------
 */

#include <QtCore>
#include "RowsJoinerProxy.h"

class RowsJoinerProxy::Private {
public:
    RowsJoinerProxy * instance;
    QList<QAbstractItemModel *> models;
    int columnCount;
    int rowCount;

    struct Addr;
    typedef QSharedPointer<Addr> AddrPtr;
    typedef QList<AddrPtr> AddrRow;
    typedef QList<AddrRow> AddrMatrix;

    struct Addr {
        Addr(const QAbstractItemModel * _m, void * _p)
            :m(_m),p(_p),root(true) {}

        const QAbstractItemModel * m;
        void * p;

        bool root;
        AddrMatrix down;
    };

    AddrMatrix tops;

    int topRowShift(const QAbstractItemModel *) const;
    void checkExpand(AddrMatrix &, int row, int col);
    AddrPtr initAddrFromSource(QModelIndex source);
};

RowsJoinerProxy::RowsJoinerProxy(QObject * parent)
:QAbstractItemModel(parent)
{
    d = new Private;
    d->instance = this;
    d->columnCount = -1;
    d->rowCount = -1;
}

RowsJoinerProxy::~RowsJoinerProxy() {
    while(d->models.size())
        removeSourceModel(d->models.back());
    delete d;
}

QList<QAbstractItemModel *> RowsJoinerProxy::models() const { return d->models; }
int RowsJoinerProxy::indexOf(QAbstractItemModel * m) const { return d->models.indexOf(m); }

void RowsJoinerProxy::insertSourceModel(QAbstractItemModel * m, int idx)
{
    if (!m)
        return;

    beginResetModel();
    if (idx < 0)
        d->models.append(m);
    else d->models.insert(idx, m);

    connect(m,  &QAbstractItemModel::rowsAboutToBeInserted,
            this, &RowsJoinerProxy::s_rowsAboutToBeInserted);
    connect(m,  &QAbstractItemModel::rowsInserted,
            this, &RowsJoinerProxy::s_rowsInserted);

    connect(m,  &QAbstractItemModel::rowsAboutToBeRemoved,
            this, &RowsJoinerProxy::s_rowsAboutToBeRemoved);
    connect(m,  &QAbstractItemModel::rowsRemoved,
            this, &RowsJoinerProxy::s_rowsRemoved);

    connect(m,  &QAbstractItemModel::dataChanged,
            this, &RowsJoinerProxy::s_dataChanged);
    connect(m,  &QAbstractItemModel::modelReset,
            this, &RowsJoinerProxy::s_modelReset);

    connect(m,  &QAbstractItemModel::layoutAboutToBeChanged,
            this, &RowsJoinerProxy::layoutAboutToBeChanged);
    connect(m,    &QAbstractItemModel::layoutChanged,
            this, &RowsJoinerProxy::layoutChanged);

    connect(m,  &QAbstractItemModel::destroyed,
            this, &RowsJoinerProxy::s_destroyed);

    d->columnCount = -1;
    d->rowCount = -1;
    d->tops.clear();
    endResetModel();
}

void RowsJoinerProxy::removeSourceModel(QAbstractItemModel * m)
{
    if (!m)
        return;

    beginResetModel();
    disconnect(m,  &QAbstractItemModel::rowsAboutToBeInserted,
               this, &RowsJoinerProxy::s_rowsAboutToBeInserted);
    disconnect(m,  &QAbstractItemModel::rowsInserted,
               this, &RowsJoinerProxy::s_rowsInserted);

    disconnect(m,  &QAbstractItemModel::rowsAboutToBeRemoved,
               this, &RowsJoinerProxy::s_rowsAboutToBeRemoved);
    disconnect(m,  &QAbstractItemModel::rowsRemoved,
               this, &RowsJoinerProxy::s_rowsRemoved);

    disconnect(m,  &QAbstractItemModel::dataChanged,
               this, &RowsJoinerProxy::s_dataChanged);
    disconnect(m,  &QAbstractItemModel::modelReset,
               this, &RowsJoinerProxy::s_modelReset);

    disconnect(m,    &QAbstractItemModel::layoutAboutToBeChanged,
               this, &RowsJoinerProxy::layoutAboutToBeChanged);
    disconnect(m,    &QAbstractItemModel::layoutChanged,
               this, &RowsJoinerProxy::layoutChanged);

    disconnect(m,  &QAbstractItemModel::destroyed,
               this, &RowsJoinerProxy::s_destroyed);

    d->models.removeAll(m);
    d->columnCount = -1;
    d->rowCount = -1;
    d->tops.clear();
    endResetModel();
}

int RowsJoinerProxy::Private::topRowShift(const QAbstractItemModel * model) const
{
    int row_shift = 0;
    QAbstractItemModel * m = const_cast<QAbstractItemModel *>(model);
    int model_idx = models.indexOf(m);
    for (int idx =0; idx< model_idx; ++idx)
        row_shift += models[idx]->rowCount();

    return row_shift;
}

void RowsJoinerProxy::Private::checkExpand(RowsJoinerProxy::Private::AddrMatrix & m, int row, int col)
{
    while (m.size() <= row) {
        Private::AddrRow empty_row;
        for(int i= 0; i<= col; ++i)
            empty_row.append(Private::AddrPtr());
        m.append(empty_row);
    }

    while (m[row].size() <= col) {
        m[row].append(Private::AddrPtr());
    }
}

RowsJoinerProxy::Private::AddrPtr RowsJoinerProxy::Private::initAddrFromSource(QModelIndex i)
{
    if (!i.isValid())
        return AddrPtr();

    int row = i.row();
    int col = i.column();

    if (!i.parent().isValid()) {
        row += topRowShift(i.model());
        checkExpand(tops, row, col);
        if (tops[row][col])
            return tops[row][col];

        AddrPtr addr(new Addr(i.model(), i.internalPointer()));
        tops[row][col] = addr;
        return addr;
    }

    AddrPtr p = initAddrFromSource(i.parent());
    checkExpand(p->down, row, col);
    if (p->down[row][col])
        return p->down[row][col];

    AddrPtr addr(new Addr(i.model(), i.internalPointer()));
    p->down[row][col] = addr;
    addr->root = false;
    return addr;
}

/*!	Mapping source model index into exact clone index of the proxy model,
    except top-level indexes which are shifted for aappropriate rows.
    Uses same internal id or internal pointer to use parent-children
    relations logic of source models.
    */
QModelIndex RowsJoinerProxy::mapFromSource(const QModelIndex & s) const
{
    if (!s.isValid())
        return QModelIndex();

    Private::AddrPtr addr = d->initAddrFromSource(s);

    int row_shift = 0;
    if (!s.parent().isValid())
        row_shift = d->topRowShift(s.model());

    return createIndex(s.row() +row_shift, s.column(), addr.data());
}

/*!	Mapping index of proxy into appropriate index of source model.
    It creates exact clones of indexes with internal id or internal pointer
    to use parent-children relations logic of source models.
    */
QModelIndex RowsJoinerProxy::mapToSource(const QModelIndex & proxy) const
{
    if (!proxy.isValid())
        return QModelIndex();

    Private::Addr * addr = reinterpret_cast<Private::Addr *>(proxy.internalPointer());
    if (!addr) return QModelIndex();

    QAbstractItemModel * m = const_cast<QAbstractItemModel *>(addr->m);
    if (!m) return QModelIndex();

    int	row_shift = 0;
    if (addr->root) {
        row_shift = -d->topRowShift(addr->m);
        if (proxy.column() >= m->columnCount())
            return QModelIndex();
    }

    // safe trick to access createIndex: needed in vs2010
    RowsJoinerProxy * trick = reinterpret_cast<RowsJoinerProxy *>(m);
    return trick->createIndex(proxy.row() +row_shift, proxy.column(), addr->p);
}

/*!	Creates exact clones of parents of source models, except
    top level where indexes should be shifted to appropriate rows.
    */
QModelIndex	RowsJoinerProxy::parent(const QModelIndex & i) const
{
    if (!i.isValid())
        return QModelIndex();

    QModelIndex si = mapToSource(i);
    if (!si.isValid() || !si.model())
        return QModelIndex();

    QModelIndex psi = si.model()->parent(si);
    if (!psi.isValid())
        return QModelIndex();

    QModelIndex pi = mapFromSource(psi);
    return pi;
}

/*!	Creates exact clones of indexes of source models, except
    top level where indexes should be shifted to appropriate rows.
    */
QModelIndex RowsJoinerProxy::index(int row, int column, const QModelIndex & p) const
{
    if (!p.isValid()) { // top level, we need to create clone of source index with shift
        int mod_idx = -1;
        int top_row = row;
        QAbstractItemModel * source_model = 0L;

        for (int idx =0; idx< d->models.size(); ++idx) {
            source_model = d->models[idx];
            if (source_model->rowCount() > top_row) {
                mod_idx = idx;
                break;
            }
            top_row -= source_model->rowCount();
        }

        if (mod_idx <0) // can not create index out of rows
            return QModelIndex();

        if (!source_model)
            return QModelIndex();

        QModelIndex si = source_model->index(top_row, column, QModelIndex());
        return mapFromSource(si);
    }

    QModelIndex sp = mapToSource(p);
    if (!sp.isValid())
        return QModelIndex();

    QModelIndex si = sp.model()->index(row, column, sp);
    return mapFromSource(si);
}

/*!	Return sum of row counts for top level.
    For nested levels it returns row count of appropriate
    models. The top level count is cached.
    */
int	RowsJoinerProxy::rowCount(const QModelIndex & p) const
{
    if (!p.isValid()) {
        if (d->rowCount <0) {
            d->rowCount =0;
            foreach(QAbstractItemModel * m, d->models)
                d->rowCount += m->rowCount();
        }
        return d->rowCount;
    }

    QModelIndex sp = mapToSource(p);
    if (sp.isValid() && sp.model())
        return sp.model()->rowCount(sp);

    return 0;
}

/*!	Return maximum of columns counts for top level.
    For nested levels it returns column count of appropriate
    models. The top level count is cached.
    */
int	RowsJoinerProxy::columnCount(const QModelIndex & p) const
{
    if (!p.isValid()) {
        if (d->columnCount <0) {
            d->columnCount = 0;
            foreach(QAbstractItemModel * m, d->models)
                d->columnCount = qMax(d->columnCount, m->columnCount());
        }
        return d->columnCount;
    }

    QModelIndex sp = mapToSource(p);
    if (sp.isValid() && sp.model())
        return sp.model()->columnCount(sp);

    return 0;
}

void RowsJoinerProxy::s_rowsAboutToBeInserted(QModelIndex sp, int from, int to)
{
    QAbstractItemModel * m = dynamic_cast<QAbstractItemModel *>(sender());
    if (!m) return;
    int f = from;
    int t = to;

    if (!sp.isValid()) {
        f += d->topRowShift(m);
        t = f + (to-from);
    }

    beginInsertRows(mapFromSource(sp), f, t);

    if (sp.isValid()) {
        Private::AddrPtr addr = d->initAddrFromSource(sp);
        Private::AddrRow row;
        for(int i= 0; i< m->columnCount(sp); ++i)
            row.append(Private::AddrPtr());
        for(int i= f; i<= t; ++i)
            addr->down.insert(f, row);
    }
    else {
        Private::AddrRow row;
        for(int i= 0; i< m->columnCount(sp); ++i)
            row.append(Private::AddrPtr());
        for(int i= f; i<= t; ++i)
            d->tops.insert(f, row);
        d->rowCount += t-f+1;
    }
}

void RowsJoinerProxy::s_rowsInserted(QModelIndex, int, int) { endInsertRows(); }

void RowsJoinerProxy::s_rowsAboutToBeRemoved(QModelIndex sp, int from, int to)
{
    QAbstractItemModel * m = dynamic_cast<QAbstractItemModel *>(sender());
    if (!m) return;
    int f = from;
    int t = to;

    if (!sp.isValid()) {
        f += d->topRowShift(m);
        t = f + (to-from);
    }

    beginRemoveRows(mapFromSource(sp), f, t);

    if (sp.isValid()) {
        Private::AddrPtr addr = d->initAddrFromSource(sp);
        for(int i= f; i<= t; ++i) {
            addr->down.removeAt(f);
        }
    }
    else {
        for(int i= f; i<= t; ++i)
            d->tops.removeAt(f);
        d->rowCount -= t-f+1;
    }
}

void RowsJoinerProxy::s_rowsRemoved(QModelIndex, int, int) { endRemoveRows(); }

void RowsJoinerProxy::s_dataChanged(QModelIndex tl, QModelIndex br)
{
    QModelIndex p_tl = mapFromSource(tl);
    QModelIndex p_br = mapFromSource(br);
    emit dataChanged(p_tl, p_br);
}

void RowsJoinerProxy::s_modelReset()
{
    beginResetModel();
    d->columnCount = -1;
    d->rowCount = -1;
    d->tops.clear();
    endResetModel();
}

void RowsJoinerProxy::s_destroyed(QObject * obj)
{
    foreach(QAbstractItemModel * m, d->models) {
        if (m == obj) {
            beginResetModel();
            d->models.removeAll(m);
            d->columnCount = -1;
            d->rowCount = -1;
            d->tops.clear();
            endResetModel();
            break;
        }
    }
}
