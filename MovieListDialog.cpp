#include "MovieListDialog.h"
#include "ui_MovieListDialog.h"

#include "Manager.h"

/**
 * @brief MovieListDialog::MovieListDialog
 * @param parent
 */
MovieListDialog::MovieListDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MovieListDialog)
{
    ui->setupUi(this);
    ui->movies->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
#ifdef Q_WS_MAC
    setWindowFlags((windowFlags() & ~Qt::WindowType_Mask) | Qt::Sheet);
    setStyleSheet(styleSheet() + " #MovieListDialog { border: 1px solid rgba(0, 0, 0, 100); border-top: none; }");
#else
    setWindowFlags((windowFlags() & ~Qt::WindowType_Mask) | Qt::Dialog);
#endif
    connect(ui->buttonClose, SIGNAL(clicked()), this, SLOT(reject()));
    connect(ui->movies, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(onMovieSelected(QTableWidgetItem*)));
}

/**
 * @brief MovieListDialog::~MovieListDialog
 */
MovieListDialog::~MovieListDialog()
{
    delete ui;
}

/**
 * @brief Returns an instance of MovieListDialog
 * @param parent Parent widget (used the first time for constructing)
 * @return Instance of MovieListDialog
 */
MovieListDialog* MovieListDialog::instance(QWidget *parent)
{
    static MovieListDialog *m_instance = 0;
    if (m_instance == 0) {
        m_instance = new MovieListDialog(parent);
    }
    return m_instance;
}

/**
 * @brief Executes the dialog
 * @return Result of QDialog::exec
 */
int MovieListDialog::exec()
{
    QSize newSize;
    newSize.setHeight(parentWidget()->size().height()-200);
    newSize.setWidth(qMin(1000, parentWidget()->size().width()-400));
    resize(newSize);

    int xMove = (parentWidget()->size().width()-size().width())/2;
    QPoint globalPos = parentWidget()->mapToGlobal(parentWidget()->pos());
    move(globalPos.x()+xMove, globalPos.y());

    ui->movies->clearContents();
    ui->movies->setRowCount(0);
    foreach (Movie *movie, Manager::instance()->movieModel()->movies()) {
        int row = ui->movies->rowCount();
        ui->movies->insertRow(row);
        QString title = (movie->released().isValid()) ? QString("%1 (%2)").arg(movie->name()).arg(movie->released().toString("yyyy")) : movie->name();
        ui->movies->setItem(row, 0, new QTableWidgetItem(title));
        ui->movies->item(row, 0)->setData(Qt::UserRole, QVariant::fromValue(movie));
    }
    return QDialog::exec();
}

/**
 * @brief Stores the selected movie and accepts the dialog
 * @param item Clicked item in the list of movies
 */
void MovieListDialog::onMovieSelected(QTableWidgetItem *item)
{
    m_selectedMovie = item->data(Qt::UserRole).value<Movie*>();
    accept();
}

/**
 * @brief Returns the last selected movie
 * @return Last selected movie
 */
Movie *MovieListDialog::selectedMovie()
{
    return m_selectedMovie;
}