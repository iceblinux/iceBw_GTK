/*$Id: rpod_l.c,v 1.25 2013/08/13 06:10:25 sasa Exp $*/
/*12.05.2015	29.04.2004	Белых А.И.	rpod_l.c
Работа со списком подразделений для подсистемы выписки счетов
*/
#include        <stdlib.h>
#include        <errno.h>
#include        <time.h>
#include        <unistd.h>
#include        <pwd.h>
#include  "i_rest.h"
#include  "rpod.h"


enum
{
  COL_KOD,
  COL_NAIM,
  COL_DATA_VREM,
  COL_KTO,  
  NUM_COLUMNS
};

gboolean   rpod_key_press(GtkWidget *widget,GdkEventKey *event,class rpod_data *data);
void rpod_vibor(GtkTreeSelection *selection,class rpod_data *data);
void rpod_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class rpod_data *data);
void  rpod_knopka(GtkWidget *widget,class rpod_data *data);
void rpod_add_columns (GtkTreeView *treeview);
void rpod_v(class rpod_data *data);
void rpod_p(class rpod_data *data);
void rpod_udzap(class rpod_data *data);
int  rpod_prov_row(SQL_str row,class rpod_data *data);
void rpod_rasp(class rpod_data *data);
int  rpod_pvu(const char *kod,GtkWidget *wpredok);


int   rpod_l(int metka_rr,iceb_u_str *kod,iceb_u_str *naim,GtkWidget *wpredok)
{
rpod_data data;
char bros[300];

data.vvod.clear_zero();
data.poisk.clear_zero();
data.metka_rr=metka_rr;

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

//gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);


sprintf(bros,"%s %s",name_system,gettext("Список подразделений"));

gtk_window_set_title (GTK_WINDOW (data.window),bros);
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

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(rpod_key_press),&data);
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

data.label_kolstr=gtk_label_new (gettext("Список подразделений"));


gtk_box_pack_start (GTK_BOX (vbox2),data.label_kolstr,FALSE, FALSE, 0);

gtk_widget_show(vbox1);
gtk_widget_show(vbox2);

data.label_poisk=gtk_label_new ("");

gtk_box_pack_start (GTK_BOX (vbox2),data.label_poisk,FALSE, FALSE, 0);

data.sw = gtk_scrolled_window_new (NULL, NULL);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);
gtk_box_pack_end (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);

//Кнопки

sprintf(bros,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2], TRUE, TRUE, 0);
g_signal_connect(data.knopka[FK2], "clicked",G_CALLBACK(rpod_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Ввод новой записи"));
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_widget_show(data.knopka[FK2]);

sprintf(bros,"%sF2 %s",RFK,gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka[SFK2]),FALSE);//Недоступна
g_signal_connect(data.knopka[SFK2], "clicked",G_CALLBACK(rpod_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[SFK2],gettext("Корректировка выбранной запси"));
gtk_widget_set_name(data.knopka[SFK2],iceb_u_inttochar(SFK2));
gtk_widget_show(data.knopka[SFK2]);


sprintf(bros,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka[FK3]),FALSE);//Недоступна
g_signal_connect(data.knopka[FK3], "clicked",G_CALLBACK(rpod_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK3],gettext("Удаление выбранной запси"));
gtk_widget_set_name(data.knopka[FK3],iceb_u_inttochar(FK3));
gtk_widget_show(data.knopka[FK3]);

sprintf(bros,"F4 %s",gettext("Поиск"));
data.knopka[FK4]=gtk_button_new_with_label(bros);
g_signal_connect(data.knopka[FK4], "clicked",G_CALLBACK(rpod_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Поиск нужных записей"));
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_widget_show(data.knopka[FK4]);

sprintf(bros,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK5], "clicked",G_CALLBACK(rpod_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK5],gettext("Распечатка записей"));
gtk_widget_set_name(data.knopka[FK5],iceb_u_inttochar(FK5));
gtk_widget_show(data.knopka[FK5]);

sprintf(bros,"F6 %s",gettext("Выбор"));
data.knopka[FK6]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK6],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK6], "clicked",G_CALLBACK(rpod_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK6],gettext("Выбор отмеченной записи"));
gtk_widget_set_name(data.knopka[FK6],iceb_u_inttochar(FK6));
gtk_widget_show(data.knopka[FK6]);

sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10], "clicked",G_CALLBACK(rpod_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);

gtk_widget_grab_focus(data.knopka[FK10]);

rpod_create_list(&data);

//gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));
gtk_widget_show(data.window);

if(metka_rr == 0)
 {
  gtk_widget_hide(data.knopka[FK6]); //спрятать кнопку
  gtk_window_maximize(GTK_WINDOW(data.window));
 }
gtk_main();

if(data.metka_voz == 0)
 {
  kod->new_plus(data.kodv.ravno());
  naim->new_plus(data.naimv.ravno());
 }

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.metka_voz);

}


/***********************************/
/*Создаем список для просмотра */
/***********************************/
void rpod_create_list (class rpod_data *data)
{
GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
char strsql[300];
int  kolstr=0;
SQL_str row;

data->kl_shift=0; //0-отжата 1-нажата  

gdk_window_set_cursor(gtk_widget_get_window(data->window),gdk_cursor_new_for_display(gtk_widget_get_display(data->window),ICEB_CURSOR_GDITE));

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(rpod_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(rpod_vibor),data);

gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (data->treeview)),GTK_SELECTION_SINGLE);




model = gtk_list_store_new (NUM_COLUMNS+1, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_INT);


sprintf(strsql,"select * from Restpod order by kod asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
//gtk_list_store_clear(model);

iceb_u_str s1;
iceb_u_str s2;
iceb_u_str s3;
iceb_u_str s4;

data->kolzap=0;

while(cur.read_cursor(&row) != 0)
 {
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  
  if(rpod_prov_row(row,data) != 0)
    continue;


  if(iceb_u_SRAV(data->vvod.kod.ravno(),row[0],0) == 0)
    data->snanomer=data->kolzap;

  //Код
  s1.new_plus(row[0]);
  
  //Наименование
  s2.new_plus(row[1]);

  //Дата и время записи
  s3.new_plus(iceb_u_vremzap(row[3]));

  //Кто записал
  s4.new_plus(iceb_u_kszap(row[2],0));

  
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_KOD,s1.ravno(),
  COL_NAIM,s2.ravno(),
  COL_DATA_VREM,s3.ravno(),
  COL_KTO,s4.ravno(),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

rpod_add_columns (GTK_TREE_VIEW (data->treeview));


if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK6]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK6]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),TRUE);//Доступна
 }

data->vvod.clear_zero();

gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

if(data->snanomer > -2)
 {
  if(data->snanomer  > data->kolzap)
     data->snanomer= data->kolzap;
      
  printf("poz=%d\n",data->snanomer);

  sprintf(strsql,"%d",data->snanomer);
  GtkTreePath *path=gtk_tree_path_new_from_string(strsql);

//  gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW (data->treeview),path,NULL,TRUE,0,0);
  gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW (data->treeview),path,NULL,FALSE,0,0);

  gtk_tree_view_set_cursor(GTK_TREE_VIEW (data->treeview),path,NULL,FALSE);
  gtk_tree_path_free(path);
  data->snanomer=-2;
 }
else
 {
  if(data->kolzap > 0)
   {
    GtkTreePath *path=gtk_tree_path_new_from_string("0");

    gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW (data->treeview),path,NULL,FALSE,0,0);

    gtk_tree_view_set_cursor(GTK_TREE_VIEW (data->treeview),path,NULL,FALSE);
  //  gtk_tree_view_row_activated(GTK_TREE_VIEW (data->treeview),path,NULL);

    gtk_tree_path_free(path);
    data->snanomer=-2;
   }
 }

iceb_u_str stroka;
iceb_u_str zagolov;
zagolov.plus(gettext("Список подразделений"));

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);

gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno());

printf("data->metka_poi=%d\n",data->metka_poi);
if(data->metka_poi == 1)
 {
  printf("Формирование заголовка с реквизитами поиска.\n");
  
  zagolov.new_plus(gettext("Поиск"));
  zagolov.plus(" !!!");
  
  if(data->poisk.kod.getdlinna() > 1)  
   {
    stroka.new_plus(gettext("Код подразделения"));
    stroka.plus(": ");
    stroka.plus(data->poisk.kod.ravno());
    zagolov.ps_plus(stroka.ravno());

   }

  if(data->poisk.naim.getdlinna() > 1)  
   {
    stroka.new_plus(gettext("Наименование подразделения"));
    stroka.plus(": ");
    stroka.plus(data->poisk.naim.ravno());
    zagolov.ps_plus(stroka.ravno());
   }

  iceb_label_set_text_color(data->label_poisk,zagolov.ravno(),"red");
  gtk_widget_show(data->label_poisk);
 }
else
 gtk_widget_hide(data->label_poisk); 

gtk_widget_show(data->label_kolstr);

gdk_window_set_cursor(gtk_widget_get_window(data->window),gdk_cursor_new_for_display(gtk_widget_get_display(data->window),ICEB_CURSOR));


}

/*****************/
/*Создаем колонки*/
/*****************/

void rpod_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Код подразделения"), renderer,"text", COL_KOD,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Наименование подразделения"), renderer,"text", COL_NAIM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Дата и время записи"), renderer,
"text", COL_DATA_VREM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Кто записал"), renderer,
"text", COL_KTO,NULL);
printf("rpod_add_columns end\n");

}

/****************************/
/*Выбор строки*/
/**********************/

void rpod_vibor(GtkTreeSelection *selection,class rpod_data *data)
{
printf("rpod_vibor\n");
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *kod;
gchar *naim;
gint  nomer;


gtk_tree_model_get(model,&iter,0,&kod,1,&naim,NUM_COLUMNS,&nomer,-1);

data->kodv.new_plus(kod);
data->naimv.new_plus(naim);
data->nomervsp=nomer;

g_free(kod);
g_free(naim);

//printf("%s %s %d\n",data->kodv.ravno(),data->naimv.ravno(),nomer);

}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  rpod_knopka(GtkWidget *widget,class rpod_data *data)
{
//iceb_u_str repl;

int knop=atoi(gtk_widget_get_name(widget));
//g_print("rpod_knopka knop=%d\n",knop);
data->kl_shift=0; //Сбрасываем нажатый сшифт так как при после запуска нового меню он не сбрасывается

switch ((gint)knop)
 {
  case FK2:
    data->metkazapisi=0;

    printf("rpod_knopka F2 metkazapisi=%d\n",data->metkazapisi);

    rpod_v(data);
    return;  

  case SFK2:
    data->metkazapisi=1;
    rpod_v(data);
    return;  

  case FK3:
    if(data->kolzap == 0)
      return;
    rpod_udzap(data);
    return;  


  case FK4:
    rpod_p(data);
    return;  

  case FK5:
//    gtk_widget_hide(data->window);
//    rpod_rasspis(data);
//    gtk_widget_show(data->window);
    rpod_rasp(data);
    return;  

  case FK6: //Подтверждение выбора нужной записи
    if(data->metka_rr == 0)
     return;
    data->metka_voz=0;
    gtk_widget_destroy(data->window);
    return;  
    
  case FK10:
//    printf("rpod_knopka F10\n");
    data->metka_voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   rpod_key_press(GtkWidget *widget,GdkEventKey *event,class rpod_data *data)
{
iceb_u_str repl;
printf("rpod_key_press keyval=%d state=%d\n",
event->keyval,event->state);

switch(event->keyval)
 {

  case GDK_KEY_F2:
    if(data->kl_shift == 0)
      g_signal_emit_by_name(data->knopka[FK2],"clicked");
    else
      g_signal_emit_by_name(data->knopka[SFK2],"clicked");

    return(TRUE);
   
  case GDK_KEY_F3:
    g_signal_emit_by_name(data->knopka[FK3],"clicked");
    return(TRUE);

  case GDK_KEY_F4:
    g_signal_emit_by_name(data->knopka[FK4],"clicked");
    return(TRUE);

  case GDK_KEY_F5:
    g_signal_emit_by_name(data->knopka[FK5],"clicked");
    return(TRUE);

  case GDK_KEY_F6:
    g_signal_emit_by_name(data->knopka[FK6],"clicked");
    return(TRUE);

  
  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka[FK10],"clicked");
    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
    printf("rpod_key_press-Нажата клавиша Shift\n");

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
void rpod_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class rpod_data *data)
{
printf("rpod_v_row\n");
printf("rpod_v_row корректировка\n");
if(data->metka_rr == 0)
 {
  g_signal_emit_by_name(data->knopka[SFK2],"clicked");
  return;
 }

gtk_widget_destroy(data->window);

data->metka_voz=0;

}

/*****************************/
/*Удаление записи            */
/*****************************/

void rpod_udzap(class rpod_data *data)
{

iceb_u_str sp;
sp.plus(gettext("Удалить запись ? Вы уверены ?"));
if(iceb_menu_danet(&sp,2,data->window) == 2)
 return;

if(rpod_pvu(data->kodv.ravno(),data->window) != 0)
 return;

char strsql[300];

sprintf(strsql,"delete from Restpod where kod=%d",
atoi(data->kodv.ravno()));

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return;


if(data->nomervsp > 0)
  data->snanomer=data->nomervsp-1;

rpod_create_list(data);

}
/****************************/
/*Проверка записей          */
/*****************************/

int  rpod_prov_row(SQL_str row,class rpod_data *data)
{
if(data->metka_poi == 0)
 return(0);

//Полное сравнение
if(iceb_u_proverka(data->poisk.kod.ravno(),row[0],0,0) != 0)
 return(1);


//Поиск образца в строке
if(data->poisk.naim.ravno()[0] != '\0' && iceb_u_strstrm(row[1],data->poisk.naim.ravno()) == 0)
 return(1);


   
return(0);
}
/*************************************/
/*Распечатка записей                 */
/*************************************/
void rpod_rasp(class rpod_data *data)
{
char strsql[300];
SQL_str row;
FILE *ff;
SQLCURSOR cur;
iceb_u_spisok imaf;
iceb_u_spisok naimot;
int kolstr=0;

sprintf(strsql,"select * from Restpod order by kod asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }

sprintf(strsql,"gkont%d.lst",getpid());

imaf.plus(strsql);
naimot.plus(gettext("Список подразделений"));

if((ff = fopen(strsql,"w")) == NULL)
 {
  iceb_er_op_fil(strsql,"",errno,data->window);
  return;
 }
iceb_u_startfil(ff);

iceb_u_zagolov(gettext("Список подразделений"),0,0,0,0,0,0,iceb_get_pnk("00",0,data->window),ff);

fprintf(ff,"\
------------------------------------------------------------------------\n");

fprintf(ff,gettext("\
Код |Наименование подраз.|Дата и время запис.| Кто записал\n"));

fprintf(ff,"\
------------------------------------------------------------------------\n");

iceb_u_str s5;
iceb_u_str s6;


cur.poz_cursor(0);
while(cur.read_cursor(&row) != 0)
 {
  if(rpod_prov_row(row,data) != 0)
    continue;

  //Дата и время записи
  s5.new_plus(iceb_u_vremzap(row[3]));

  //Кто записал
  s6.new_plus(iceb_u_kszap(row[2],1));

  fprintf(ff,"%-4s %-20.20s %s %s\n",
  row[0],row[1],s5.ravno(),s6.ravno());


 }
fprintf(ff,"\
------------------------------------------------------------------------\n");

fprintf(ff,"%s: %d\n",gettext("Количество записей"),kolstr);
iceb_podpis(ff,data->window);

fclose(ff);

iceb_rabfil(&imaf,&naimot,data->window);

}
/*****************************/
/*Проверить возможность удаления*/
/********************************/
//Если вернули 0-можно удалять 1-нет
int  rpod_pvu(const char *kod,GtkWidget *wpredok)
{
char strsql[300];
sprintf(strsql,"select podr from Restdok where podr=%s limit 1",kod);
if(sql_readkey(&bd,strsql) > 0)
 {
  iceb_u_str repl;
  repl.plus(gettext("Подразделение удалить нельзя ! Есть заказы с этим кодом подразделения !"));
  iceb_menu_soob(&repl,wpredok);
  return(1);
 }
return(0);

}
