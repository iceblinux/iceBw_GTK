/*$Id: l_spisok.c,v 1.33 2013/08/13 06:10:24 sasa Exp $*/
/*12.05.2015	25.02.2004	Белых А.И.	l_spisok.c
Выбор из списка
  Если вернули 0-выбрали
               1-не выбрали
*/
#include <stdlib.h>
#include "i_rest.h"
enum
{
  FK2,
  FK_VVERH,
  FK_VNIZ,
  FK10,
  KOL_F_KL
};

enum
{
 COL_KOD,
 COL_NAIM,
 NUM_COLUMNS
};
 
class  spisok_data
 {
  public:
  int        skl;  // склад для которого показываем список групп 
  int podr; //подразделение для которого нужно выбрать список складов
  iceb_u_str kod; //выбранный код записи
  int        nomervsp; //Номер записи в списке
  short      metka_sp; //0-список складов 1-список групп товаров 2-список подразделений 3-список групп услуг

  GtkWidget *label_kolstr;
  GtkWidget *sw;
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  short     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать
  int       kolzap;     //Количество записей
  int       voz;     //0-код выбран 1-нет  

  short metka_released;  //0-отжата 1-нажата
  
  //Конструктор
  spisok_data()
   {
    voz=1;
    kod.new_plus("");
    snanomer=0;
    kl_shift=0;
    window=treeview=NULL;
    skl=0;
    metka_released=0;
   }      
 };

extern SQL_baza	bd;
extern char *name_system;
extern char *imabaz;

void spisok_create_list (class spisok_data *data);
void spisok_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,class spisok_data *data);
gboolean   spisok_key_press(GtkWidget *widget,GdkEventKey *event,class spisok_data *data);
void  spisok_knopka(GtkWidget *widget,class spisok_data *data);
void spisok_add_columns(GtkTreeView *treeview);
void spisok_vibor(GtkTreeSelection *selection,class spisok_data *data);

void  l_spisok_knopka_vniz_press(GtkWidget *widget,class spisok_data *data);
void  l_spisok_knopka_vverh_press(GtkWidget *widget,class spisok_data *data);
void  l_spisok_knopka_released(GtkWidget *widget,class spisok_data *data);
void l_spisok_setstr(class spisok_data *data);


int l_spisok(int metka, //0-список складов 1-список групп 2-список подразделений в услугах 3-список групп услуг 100-список единиц измерения
int skl, //склад для списка групп
iceb_u_str *kodv, //Возвращаемый код
int podr, //подразделение для которого нужно выбрать список складов
GtkWidget *wpredok)
{
spisok_data data;
char strsql[300];
printf("l_spisok %d %d\n",metka,skl);

data.metka_sp=metka;
data.skl=skl;
data.podr=podr;

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

gtk_window_set_default_size (GTK_WINDOW  (data.window),-1,600);

if(metka == 0)
  sprintf(strsql,"%s %s",name_system,gettext("Список складов"));
if(metka == 1)
  sprintf(strsql,"%s %s",name_system,gettext("Список групп"));

if(metka == 2)
  sprintf(strsql,"%s %s",name_system,gettext("Список подразделений"));
if(metka == 3)
  sprintf(strsql,"%s %s",name_system,gettext("Список групп"));

gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);


if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(spisok_key_press),&data);
g_signal_connect_after(data.window,"key_release_event",G_CALLBACK(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 1);
gtk_box_set_homogeneous (GTK_BOX(hbox),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 1);
gtk_box_set_homogeneous (GTK_BOX(vbox1),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *vbox2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 1);
gtk_box_set_homogeneous (GTK_BOX(vbox2),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 0);
gtk_widget_show(hbox);

data.label_kolstr=gtk_label_new (gettext("Выберите нужную запись"));


gtk_box_pack_start (GTK_BOX (vbox2),data.label_kolstr,FALSE, FALSE, 0);

gtk_widget_show(vbox1);
gtk_widget_show(vbox2);

data.sw = gtk_scrolled_window_new (NULL, NULL);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);
gtk_box_pack_end (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);

//Кнопки

sprintf(strsql,"F2 %s",gettext("Выбор"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2], TRUE, TRUE, 0);
g_signal_connect(data.knopka[FK2], "clicked",G_CALLBACK(spisok_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Выбор отмеченной записи"));
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_widget_show(data.knopka[FK2]);

GtkWidget *label=gtk_label_new ("+");

data.knopka[FK_VVERH]=gtk_button_new();
gtk_container_add(GTK_CONTAINER(data.knopka[FK_VVERH]),label);

gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK_VVERH], TRUE, TRUE, 0);
g_signal_connect(data.knopka[FK_VVERH], "pressed",G_CALLBACK(l_spisok_knopka_vverh_press),&data);
g_signal_connect(data.knopka[FK_VVERH], "released",G_CALLBACK(l_spisok_knopka_released),&data);
//gtk_widget_set_tooltip_text(data.knopka[FK_VVERH],gettext("Выбор отмеченной записи"));
gtk_widget_set_name(data.knopka[FK_VVERH],iceb_u_inttochar(FK_VVERH));
gtk_widget_show(data.knopka[FK_VVERH]);
gtk_widget_show(label);

label=gtk_label_new ("-");


data.knopka[FK_VNIZ]=gtk_button_new();
gtk_container_add(GTK_CONTAINER(data.knopka[FK_VNIZ]),label);

gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK_VNIZ], TRUE, TRUE, 0);
g_signal_connect(data.knopka[FK_VNIZ], "pressed",G_CALLBACK(l_spisok_knopka_vniz_press),&data);
g_signal_connect(data.knopka[FK_VNIZ], "released",G_CALLBACK(l_spisok_knopka_released),&data);
//gtk_widget_set_tooltip_text(data.knopka[FK_VNIZ],gettext("Выбор отмеченной записи"));
gtk_widget_set_name(data.knopka[FK_VNIZ],iceb_u_inttochar(FK_VNIZ));
gtk_widget_show(data.knopka[FK_VNIZ]);
gtk_widget_show(label);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10], "clicked",G_CALLBACK(spisok_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
//gdk_window_set_cursor(gtk_widget_get_window(data.window),gdk_cursor_new(ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

spisok_create_list (&data);

//gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));
gtk_widget_show(data.window);


gtk_main();

printf("l_spisok end\n");
kodv->new_plus(data.kod.ravno());
if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));
return(data.voz);
}


/***********************************/
/*Создаем список для просмотра */
/***********************************/
void spisok_create_list (class spisok_data *data)
{
GtkListStore *model=NULL;
GtkTreeIter iter;
char strsql[512];
char spisok[2048];
char imafil[56];
SQLCURSOR cur;
SQL_str   row;

printf("spisok_create_list %d\n",data->snanomer);
data->kl_shift=0; //0-отжата 1-нажата  

gdk_window_set_cursor(gtk_widget_get_window(data->window),gdk_cursor_new_for_display(gtk_widget_get_display(data->window),ICEB_CURSOR_GDITE));

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(spisok_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(spisok_vibor),data);

gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (data->treeview)),GTK_SELECTION_SINGLE);




model = gtk_list_store_new (NUM_COLUMNS+1, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_INT);

data->kolzap=0;

//gtk_list_store_clear(model);
if(data->metka_sp < 100)
 {
  memset(spisok,'\0',sizeof(spisok));

  sprintf(imafil,"%s","restnast.alx");
  if(data->metka_sp == 0)
   {
    iceb_u_str spis_skl;
    l_restdok_spis_skl(&spis_skl,data->podr);
    strncpy(spisok,spis_skl.ravno(),sizeof(spisok)-1);
   }

  if(data->metka_sp == 1)
   {
    char bros[100];
    sprintf(bros,"Список групп товаров для склада %d",data->skl);
    iceb_poldan(bros,spisok,imafil,data->window);
   }

  if(data->metka_sp == 2)
    iceb_poldan("Список подразделений предоставляющих услуги",spisok,imafil,data->window);

  if(data->metka_sp == 3)
   {
    char bros[100];
    sprintf(bros,"Список групп услуг для подразделения %d",data->skl);
    iceb_poldan(bros,spisok,imafil,data->window);
   }

  printf("spisok=%s\n",spisok);

  int kolih=iceb_u_pole2(spisok,',');

  if(kolih == 0 && spisok[0] != '\0')
    kolih=1;


  class iceb_u_str  kod("");
  iceb_u_str naim;

  for(int i=1; i <= kolih; i++)
   {
    naim.new_plus("");
    if(kolih > 1)
      iceb_u_polen(spisok,&kod,i,',');
    else
     kod.new_plus(spisok);
     
    if(data->metka_sp == 0) //список складов
      sprintf(strsql,"select naik from Sklad where kod=%s",kod.ravno());

    if(data->metka_sp == 1) //список групп для склада
      sprintf(strsql,"select naik from Grup where kod=%s",kod.ravno());


    if(data->metka_sp == 2) //список подразделений
      sprintf(strsql,"select naik from Uslpodr where kod=%s",kod.ravno());

    if(data->metka_sp == 3) //список групп услуг
     sprintf(strsql,"select naik from Uslgrup where kod=%s",kod.ravno());
    printf("strsql=%s\n",strsql);

    if(sql_readkey(&bd,strsql,&row,&cur) == 1)
      naim.new_plus(row[0]);        
      
    
    gtk_list_store_append (model, &iter);

    gtk_list_store_set (model, &iter,
    COL_KOD,kod.ravno(),
    COL_NAIM,naim.ravno(),
    NUM_COLUMNS,data->kolzap,
    -1);

    data->kolzap++;
   }
 }
else
 {
  int kolstr;
  SQL_str row;
  SQLCURSOR cur;

  if(data->metka_sp == 100)  
   sprintf(strsql,"select kod,naik from Edizmer order by kod asc");

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    return;
   }
  //gtk_list_store_clear(model);
  iceb_u_str ss[NUM_COLUMNS];

  while(cur.read_cursor(&row) != 0)
   {
    //КОД
    ss[COL_KOD].new_plus(row[0]);

    //Наименование
    ss[COL_NAIM].new_plus(row[1]);

    gtk_list_store_append (model, &iter);

    gtk_list_store_set (model, &iter,
    COL_KOD,ss[COL_KOD].ravno(),
    COL_NAIM,ss[COL_NAIM].ravno(),
    NUM_COLUMNS,data->kolzap,
    -1);

    data->kolzap++;

   }  
 }
 
gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

spisok_add_columns (GTK_TREE_VIEW (data->treeview));


if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK2]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK2]),TRUE);//Доступна
 }


gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

if(data->kolzap > 0)
 {
  if(data->snanomer  > data->kolzap-1)
     data->snanomer= data->kolzap-1;
      
  printf("poz=%d\n",data->snanomer);

  sprintf(strsql,"%d",data->snanomer);
  GtkTreePath *path=gtk_tree_path_new_from_string(strsql);

//  gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW (data->treeview),path,NULL,TRUE,0,0);
  gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW (data->treeview),path,NULL,FALSE,0,0);

  gtk_tree_view_set_cursor(GTK_TREE_VIEW (data->treeview),path,NULL,FALSE);
  gtk_tree_path_free(path);
 }

iceb_u_str stroka;
iceb_u_str zagolov;
zagolov.plus(gettext("Выберите нужную запись"));

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);

gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno());


gtk_widget_show(data->label_kolstr);

gdk_window_set_cursor(gtk_widget_get_window(data->window),gdk_cursor_new_for_display(gtk_widget_get_display(data->window),ICEB_CURSOR));


}

/*****************/
/*Создаем колонки*/
/*****************/

void spisok_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;


renderer = gtk_cell_renderer_text_new ();

gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
					       -1,gettext("Код"), renderer,
					       "text", COL_KOD,
					       NULL);

renderer = gtk_cell_renderer_text_new ();


gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,\
gettext("Наименование"), renderer,"text", COL_NAIM,NULL);



}

/****************************/
/*Выбор строки*/
/**********************/

void spisok_vibor(GtkTreeSelection *selection,class spisok_data *data)
{
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *kod;
gint  nomer;


gtk_tree_model_get(model,&iter,0,&kod,NUM_COLUMNS,&nomer,-1);


data->kod.new_plus(kod);
data->nomervsp=nomer;

g_free(kod);


}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  spisok_knopka(GtkWidget *widget,class spisok_data *data)
{
iceb_u_str repl;

int knop=atoi(gtk_widget_get_name(widget));
data->kl_shift=0; //Сбрасываем нажатый сшифт так как при после запуска нового меню он не сбрасывается

switch ((gint)knop)
 {
  case FK2:
    if(data->kolzap == 0)
      return;
    data->voz=0;
    gtk_widget_destroy(data->window);
    return;  

  case FK10:
    gtk_widget_destroy(data->window);
    data->voz=1;
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   spisok_key_press(GtkWidget *widget,GdkEventKey *event,class spisok_data *data)
{

switch(event->keyval)
 {

  case GDK_KEY_F2:
    g_signal_emit_by_name(data->knopka[FK2],"clicked");
    return(TRUE);
   
  
  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka[FK10],"clicked");
    return(TRUE);

  case ICEB_REG_L:
  case ICEB_REG_R:
    printf("spisok_key_press-Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);

  default:
    printf("Не выбрана клавиша !\n");
    break;
 }

return(TRUE);
}
/****************************/
/*Выбор строки*/
/**********************/
void spisok_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class spisok_data *data)
{
printf("spisok_v_row\n");
 g_signal_emit_by_name(data->knopka[FK2],"clicked");
}
/*************************/
/*Кнопка вниз нажата*/
/**********************************/
void  l_spisok_knopka_vniz_press(GtkWidget *widget,class spisok_data *data)
{
data->metka_released=1;
while(data->metka_released == 1)
 {
  data->snanomer++;
  printf("вниз-%d\n",data->snanomer);

  l_spisok_setstr(data); //стать на нужную строку

  g_usleep(SLEEP_STR);

  iceb_refresh();  //обязательно после sleep
 }


}
/*************************/
/*Кнопка вверх нажата*/
/**********************************/

void  l_spisok_knopka_vverh_press(GtkWidget *widget,class spisok_data *data)
{
data->metka_released=1;
while(data->metka_released == 1)
 {
  data->snanomer--;
  printf("вверх-%d\n",data->snanomer);

  l_spisok_setstr(data); //стать на нужную строку


  g_usleep(SLEEP_STR);

  iceb_refresh();  //обязательно после sleep
 }


}
/*************************/
/*Кнопка вниз/вверх отжата*/
/**********************************/
void  l_spisok_knopka_released(GtkWidget *widget,class spisok_data *data)
{
data->metka_released=0;
}

/***********************************/
/*Стат на нужную строку*/
/************************************/
void l_spisok_setstr(class spisok_data *data)
{
  if(data->snanomer  > data->kolzap-1)
     data->snanomer= data->kolzap-1;
  if(data->snanomer < 0)
     data->snanomer=0;
      
char strsql[50];
  sprintf(strsql,"%d",data->snanomer);
  GtkTreePath *path=gtk_tree_path_new_from_string(strsql);

//  gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW (data->treeview),path,NULL,TRUE,0,0);
  gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW (data->treeview),path,NULL,FALSE,0,0);

  gtk_tree_view_set_cursor(GTK_TREE_VIEW (data->treeview),path,NULL,FALSE);
  gtk_tree_path_free(path);
}
