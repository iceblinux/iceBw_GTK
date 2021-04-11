/*$Id: l_ukrdok.c,v 1.23 2013/12/31 11:49:14 sasa Exp $*/
/*08.05.2015	21.02.2008	Белых А.И.	l_ukrdok.c
Работа с документом в подсистеме "Учёт командировочных расходов"
*/
#include "buhg_g.h"

enum
{
 SFK1,
 FK2,
 SFK2,
 FK3,
 SFK3,
 FK4,
 FK5,
 FK10,
 KOL_F_KL
};

enum
{
 COL_KOD,
 COL_NAIM,
 COL_SHET,
 COL_EI,
 COL_KOLIH,
 COL_STRAH_SBOR,
 COL_SUMA,
 COL_SUMA_NDS,
 COL_SUMA_DOK,
 COL_DVNN,
 COL_DATA_VREM,
 COL_KTO,  
 COL_NOMZAP,
 NUM_COLUMNS
};

class  ukrdok_data
 {
  public:
  GtkWidget *label_rek_dok;
  GtkWidget *sw;
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  short     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать 
  int       kolzap;     //Количество записей
  int voz;
  
  //реквизиты документа с которым работаем
  class iceb_u_str data_dok; /*Дата документа в символьном виде*/
  class iceb_u_str nomd;
  
  class iceb_u_str kontr;
  class iceb_u_str vidkom;
  class iceb_u_str nomd_p;
  
  /*Выбранный инвентарный номер*/
  class iceb_u_str kodz_v;
  class iceb_u_str nomzap_v;  
  class iceb_u_str shet_zap;
    
  /*заголовок меню*/
  class iceb_u_str hapka;
    
  //Конструктор
  ukrdok_data()
   {
    voz=0;
    clear_rek_dok();
    snanomer=0;
    kl_shift=0;
    window=treeview=NULL;
   }      
  void clear_rek_dok()
   {
    kontr.new_plus("");
    vidkom.new_plus("");
      
   }

 };

gboolean   ukrdok_key_press(GtkWidget *widget,GdkEventKey *event,class ukrdok_data *data);
void ukrdok_vibor(GtkTreeSelection *selection,class ukrdok_data *data);
void ukrdok_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,class ukrdok_data *data);
void  ukrdok_knopka(GtkWidget *widget,class ukrdok_data *data);
void ukrdok_add_columns (GtkTreeView *treeview);
void ukrdok_create_list(class ukrdok_data *data);

int l_ukrdok_read_rhd(class  ukrdok_data *data,GtkWidget *wpredok);
int l_ukrdok_vn(short dd,short md,short gd,const char *nomdok,GtkWidget *wpredok);
int l_ukrdok_uz(class ukrdok_data *data);
int l_ukrdok_ud(class ukrdok_data *data);
void l_prov_ukr(const char *data_dok,const char *nomdok,GtkWidget *wpredok);
void ukrkuw(short gd,const char *nomd,GtkWidget *wpredok);

extern SQL_baza	bd;

int l_ukrdok(const char *data_dok, //Дата документа
const char *nomd, //Номер документа
GtkWidget *wpredok)
{
class  ukrdok_data data;
iceb_u_str string;
char bros[512];
data.data_dok.new_plus(data_dok);
data.nomd.plus(nomd);


/*Читаем реквизиты документа*/
l_ukrdok_read_rhd(&data,wpredok);

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

//gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(bros,"%s %s",iceb_get_namesystem(),gettext("Работа с документом"));

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

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(ukrdok_key_press),&data);
g_signal_connect_after(data.window,"key_release_event",G_CALLBACK(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 1);
gtk_box_set_homogeneous (GTK_BOX(hbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 1);
gtk_box_set_homogeneous (GTK_BOX(vbox1),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *vbox2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 1);
gtk_box_set_homogeneous (GTK_BOX(vbox2),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 0);
gtk_widget_show(hbox);


data.label_rek_dok=gtk_label_new (data.hapka.ravno());
gtk_box_pack_start (GTK_BOX (vbox2),data.label_rek_dok,FALSE, FALSE, 0);
gtk_widget_show(data.label_rek_dok);




gtk_widget_show(vbox1);
gtk_widget_show(vbox2);

data.sw = gtk_scrolled_window_new (NULL, NULL);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);
gtk_box_pack_end (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);

//Кнопки

sprintf(bros,"%sF1 %s",RFK,gettext("Шапка"));
data.knopka[SFK1]=gtk_button_new_with_label(bros);
g_signal_connect(data.knopka[SFK1], "clicked",G_CALLBACK(ukrdok_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK1],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[SFK1],gettext("Просмотр шапки документа"));
gtk_widget_set_name(data.knopka[SFK1],iceb_u_inttochar(SFK1));
gtk_widget_show(data.knopka[SFK1]);

sprintf(bros,"F2 %s",gettext("Ввести"));
data.knopka[FK2]=gtk_button_new_with_label(bros);
g_signal_connect(data.knopka[FK2], "clicked",G_CALLBACK(ukrdok_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Ввод новой записи"));
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_widget_show(data.knopka[FK2]);

sprintf(bros,"%sF2 %s",RFK,gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(bros);
g_signal_connect(data.knopka[SFK2], "clicked",G_CALLBACK(ukrdok_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[SFK2],gettext("Корректировка выбранной записи"));
gtk_widget_set_name(data.knopka[SFK2],iceb_u_inttochar(SFK2));
gtk_widget_show(data.knopka[SFK2]);

sprintf(bros,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
g_signal_connect(data.knopka[FK3], "clicked",G_CALLBACK(ukrdok_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK3],gettext("Удалить выбранную запись"));
gtk_widget_set_name(data.knopka[FK3],iceb_u_inttochar(FK3));
gtk_widget_show(data.knopka[FK3]);

sprintf(bros,"%sF3 %s",RFK,gettext("Удалить"));
data.knopka[SFK3]=gtk_button_new_with_label(bros);
g_signal_connect(data.knopka[SFK3], "clicked",G_CALLBACK(ukrdok_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK3],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[SFK3],gettext("Удалить документ"));
gtk_widget_set_name(data.knopka[SFK3],iceb_u_inttochar(SFK3));
gtk_widget_show(data.knopka[SFK3]);

sprintf(bros,"F4 %s",gettext("Проводки"));
data.knopka[FK4]=gtk_button_new_with_label(bros);
g_signal_connect(data.knopka[FK4], "clicked",G_CALLBACK(ukrdok_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Переход в режим работы с проводками для этого документа"));
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_widget_show(data.knopka[FK4]);

sprintf(bros,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK5], "clicked",G_CALLBACK(ukrdok_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK5],gettext("Распечатка документов"));
gtk_widget_set_name(data.knopka[FK5],iceb_u_inttochar(FK5));
gtk_widget_show(data.knopka[FK5]);

sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10], "clicked",G_CALLBACK(ukrdok_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_widget_show(data.knopka[FK10]);

 
gtk_widget_realize(data.window);
gdk_window_set_cursor(gtk_widget_get_window(data.window),gdk_cursor_new_for_display(gtk_widget_get_display(data.window),ICEB_CURSOR));

//gtk_widget_grab_focus(data.knopka[FK10]);

ukrdok_create_list(&data);
gtk_widget_show(data.window);

gtk_window_maximize(GTK_WINDOW(data.window));


gtk_main();


//printf("l_ukrdok end\n");

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));
return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  ukrdok_knopka(GtkWidget *widget,class ukrdok_data *data)
{
int knop=atoi(gtk_widget_get_name(widget));
//g_print("ukrdok_knopka knop=%d\n",knop);

data->kl_shift=0; //Сбрасываем нажатый шифт так как после запуска нового меню он не сбрасывается
char strsql[512];
short dd,md,gd;
iceb_u_rsdat(&dd,&md,&gd,data->data_dok.ravno(),1);
switch (knop)
 {
  case SFK1:

    if(ukrvhd(&data->data_dok,&data->nomd,data->window) == 0)
     {
      /*Читаем реквизиты документа*/
      l_ukrdok_read_rhd(data,data->window);
      ukrdok_create_list(data);
      data->voz=1;
     }
    return;  

  case FK2:
    if(l_ukrdok_pb(data->data_dok.ravno(),data->window) != 0)
     return;

    if(l_ukrdok_vn(dd,md,gd,data->nomd.ravno(),data->window) == 0)
      ukrdok_create_list(data);
    return;  

  case SFK2:
    if(data->kolzap == 0)
     return;

    if(l_ukrdok_v(dd,md,gd,data->nomd.ravno(),data->shet_zap.ravno(),data->kodz_v.ravno_atoi(),data->nomzap_v.ravno_atoi(),data->window) == 0)
      ukrdok_create_list(data);

    return;  

  case FK3:
    if(data->kolzap == 0)
     return;
    if(l_ukrdok_pb(data->data_dok.ravno(),data->window) != 0)
     return;

    if(l_ukrdok_uz(data) == 0)
      ukrdok_create_list(data);

    return;  

  case SFK3:
    if(l_ukrdok_pb(data->data_dok.ravno(),data->window) != 0)
     return;

    if(l_ukrdok_ud(data) != 0)
     return;

    data->voz=1;
    gtk_widget_destroy(data->window);
    return;  


  case FK4:
    if(data->kolzap == 0)
     return;


    l_prov_ukr(data->data_dok.ravno(),data->nomd.ravno(),data->window);
      
    return;  

  case FK5:
    if(data->kolzap == 0)
     return;

    ukrkuw(data->data_dok.ravno_god(),data->nomd.ravno(),data->window);

    return;  

  case FK10:
//    printf("uosdok_knopka F10\n");

    if(data->kolzap > 0)
     sprintf(strsql,"update Ukrdok set mpodt=1 where god=%d and nomd='%s'",
     data->data_dok.ravno_god(),data->nomd.ravno());

    if(data->kolzap == 0)
     sprintf(strsql,"update Ukrdok set mpodt=0 where god=%d and nomd='%s'",
     data->data_dok.ravno_god(),data->nomd.ravno());

    iceb_sql_zapis(strsql,1,0,data->window);
      
    prosprkrw(dd,md,gd,data->nomd.ravno(),1,NULL,data->window);

    gtk_widget_destroy(data->window);
    return;


 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   ukrdok_key_press(GtkWidget *widget,GdkEventKey *event,class ukrdok_data *data)
{
//printf("ukrdok_key_press keyval=%d state=%d\n",event->keyval,event->state);

switch(event->keyval)
 {

  case GDK_KEY_F1:
//    if(data->kl_shift == 0)
//      g_signal_emit_by_name(data->knopka[FK1],"clicked");
    if(data->kl_shift == 1)
      g_signal_emit_by_name(data->knopka[SFK1],"clicked");
    return(TRUE);

  case GDK_KEY_plus: //это почемуто не работает Нажата клавиша плюс на основной клавиатуре
  case GDK_KEY_KP_Add: //Нажата клавиша "+" на дополнительной клавиатуре
  case GDK_KEY_F2:
    if(data->kl_shift == 0)
      g_signal_emit_by_name(data->knopka[FK2],"clicked");
    if(data->kl_shift == 1)
      g_signal_emit_by_name(data->knopka[SFK2],"clicked");
    return(TRUE);

  case GDK_KEY_minus: //Нажата клавиша минус на основной клавиатуре
  case GDK_KEY_KP_Subtract: //Нажата клавиша "-" на дополнительной клавиатуре
    return(TRUE);
  
  case GDK_KEY_F3:
    if(data->kl_shift == 0)
      g_signal_emit_by_name(data->knopka[FK3],"clicked");
    if(data->kl_shift == 1)
      g_signal_emit_by_name(data->knopka[SFK3],"clicked");
    return(TRUE);

  case GDK_KEY_F4:
    g_signal_emit_by_name(data->knopka[FK4],"clicked");
    return(TRUE);

  case GDK_KEY_F5:
    g_signal_emit_by_name(data->knopka[FK5],"clicked");
    return(TRUE);


  case ICEB_REG_L:
  case ICEB_REG_R:
    printf("ukrdok_key_press-Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);

  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka[FK10],"clicked");
    return(FALSE);

  default:
    printf("Не выбрана клавиша ! Код=%d\n",event->keyval);

    break;
 }

return(TRUE);
}
/*****************/
/*Создаем колонки*/
/*****************/

void ukrdok_add_columns(class ukrdok_data *data)
{
GtkCellRenderer *renderer;

//printf("ukrdok_add_columns\n");

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (data->treeview),-1,
gettext("Код расхода"), renderer,"text", COL_KOD,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (data->treeview),
-1,gettext("Наименование"), renderer,"text", COL_NAIM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (data->treeview),-1,
gettext("Счёт"), renderer,"text", COL_SHET,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (data->treeview),-1,
gettext("Ед.изм."), renderer,"text", COL_EI,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (data->treeview),-1,
gettext("Количество"), renderer,"text", COL_KOLIH,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (data->treeview),-1,
gettext("Страховой сбор"), renderer,"text", COL_STRAH_SBOR,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (data->treeview),-1,
gettext("Сумма"), renderer,"text", COL_SUMA,NULL);


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (data->treeview),-1,
gettext("Сумма НДС"), renderer,"text", COL_SUMA_NDS,NULL);


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (data->treeview),-1,
gettext("Общая сумма"), renderer,"text", COL_SUMA_DOK,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (data->treeview),-1,
gettext("Дата в.н.н"), renderer,"text", COL_DVNN,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (data->treeview),-1,
gettext("Дата и время записи"), renderer,
"text", COL_DATA_VREM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (data->treeview),-1,
gettext("Кто записал"), renderer,
"text", COL_KTO,NULL);

//printf("ukrdok_add_columns end\n");

}
/***********************************/
/*Создаем список для просмотра */
/***********************************/
void ukrdok_create_list (class ukrdok_data *data)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);

GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
SQLCURSOR cur1;
char strsql[512];
int  kolstr=0;
SQL_str row,row1;

//printf("ukrdok_create_list %d\n",data->snanomer);
data->kl_shift=0; //0-отжата 1-нажата  

gdk_window_set_cursor(gtk_widget_get_window(data->window),gdk_cursor_new_for_display(gtk_widget_get_display(data->window),ICEB_CURSOR_GDITE));
iceb_refresh();

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(ukrdok_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(ukrdok_vibor),data);

gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (data->treeview)),GTK_SELECTION_SINGLE);


model = gtk_list_store_new (NUM_COLUMNS+1, 
G_TYPE_STRING,
G_TYPE_STRING,
G_TYPE_STRING,
G_TYPE_STRING,
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING,
G_TYPE_STRING, 
G_TYPE_STRING,
G_TYPE_STRING,
G_TYPE_STRING,
G_TYPE_STRING,
G_TYPE_STRING,
G_TYPE_INT);

sprintf(strsql,"select * from Ukrdok1 where datd='%s' and nomd='%s' order by kodr asc",
data->data_dok.ravno_sqldata(),data->nomd.ravno());
/*printf("strsql=%s\n",strsql);*/
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
//gtk_list_store_clear(model);

class iceb_u_str ss[NUM_COLUMNS];

data->kolzap=0;

float kolstr1=0.;
double suma_bez_nds=0.;
double suma_nds=0.;
double i_suma_bez_nds=0.;
double i_suma_nds=0.;
double strah_sbor=0.;
double i_strah_sbor=0.;
while(cur.read_cursor(&row) != 0)
 {
  
  iceb_pbar(gdite.bar,kolstr,++kolstr1);

   
  //Узнаём наименование кода расхода

  ss[COL_NAIM].new_plus("");
  sprintf(strsql,"select naim from Ukrkras where kod=%d",atoi(row[2]));
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
    ss[COL_NAIM].new_plus(row1[0]);


  //количество
  sprintf(strsql,"%.10g",atof(row[4]));
  ss[COL_KOLIH].new_plus(strsql);
  
  suma_bez_nds=atof(row[5]);
  suma_nds=atof(row[7]);
  strah_sbor=atof(row[16]);  
  i_suma_bez_nds+=suma_bez_nds;
  i_suma_nds+=suma_nds;
  i_strah_sbor+=strah_sbor;    

  
  /*Сумма документа*/
  ss[COL_SUMA_DOK].new_plus(suma_bez_nds+suma_nds+strah_sbor);

  //Дата выписки налоговой накладной
  ss[COL_DVNN].new_plus(iceb_u_datzap(row[15]));
  

  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_KOD,row[2],
  COL_NAIM,ss[COL_NAIM].ravno(),
  COL_SHET,row[3],
  COL_EI,row[6],
  COL_KOLIH,ss[COL_KOLIH].ravno(),
  COL_SUMA,row[5],
  COL_STRAH_SBOR,row[16],
  COL_SUMA_DOK,ss[COL_SUMA_DOK].ravno(),
  COL_DVNN,ss[COL_DVNN].ravno(),
  COL_SUMA_NDS,row[7],
  COL_DATA_VREM,iceb_u_vremzap(row[9]),
  COL_KTO,iceb_kszap(row[8],data->window),
  COL_NOMZAP,row[10],
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

ukrdok_add_columns (data);

if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
 }
gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);

class iceb_u_str zagol;
zagol.plus(data->hapka.ravno());

if(data->kolzap > 0)
 {
  sprintf(strsql,"%s:%d",
  gettext("Количество записей"),
  data->kolzap);

  zagol.ps_plus(strsql);
  
  sprintf(strsql,"%s:%.10g+%.10g+%.10g=%.10g",
  gettext("Сумма"),
  i_suma_bez_nds,
  i_suma_nds,
  i_strah_sbor,
  i_suma_bez_nds+i_suma_nds+i_strah_sbor);
  
  zagol.ps_plus(strsql);

 }
 
gtk_label_set_text(GTK_LABEL(data->label_rek_dok),zagol.ravno());

gdk_window_set_cursor(gtk_widget_get_window(data->window),gdk_cursor_new_for_display(gtk_widget_get_display(data->window),ICEB_CURSOR));
}
/****************************/
/*Чтение реквизитов стороки на которую установлена подсветка строки*/
/**********************/

void ukrdok_vibor(GtkTreeSelection *selection,class ukrdok_data *data)
{
//printf("ukrdok_vibor\n");
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *kodz;
gchar *nomzap;
gchar *shet_zap;
gint  nomer;

gtk_tree_model_get(model,&iter,
COL_KOD,&kodz,
COL_NOMZAP,&nomzap,
COL_SHET,&shet_zap,
NUM_COLUMNS,&nomer,-1);

data->kodz_v.new_plus(kodz);
data->nomzap_v.new_plus(nomzap);
data->shet_zap.new_plus(shet_zap);
data->snanomer=nomer;

g_free(kodz);
g_free(shet_zap);
g_free(nomzap);
//printf("ukrdok_vibor end\n");

}
/****************************/
/*Выбор строки по двойному клику мышкой*/
/**********************/
void ukrdok_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class ukrdok_data *data)
{
//Корректировка записи
g_signal_emit_by_name(data->knopka[SFK2],"clicked");


}

/******************************/
/*Чтение реквизитов шапки документа*/
/************************************/
int l_ukrdok_read_rhd(class  ukrdok_data *data,GtkWidget *wpredok)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select * from Ukrdok where god=%d and \
nomd='%s'",data->data_dok.ravno_god(),data->nomd.ravno());

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  
  sprintf(strsql,"%s %s %s !",
  gettext("Не найден документ"),
  data->nomd.ravno(),
  data->data_dok.ravno());
  
  iceb_menu_soob(strsql,wpredok);
  return(1);
 }



class iceb_u_str nomdp;
nomdp.new_plus(row[1]);

data->kontr.new_plus(row[3]);
data->vidkom.new_plus(row[13]);
data->nomd_p.new_plus(row[1]);

/*Узнаём наименование контрагента*/
class iceb_u_str naim_kontr("");
sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",data->kontr.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 naim_kontr.new_plus(row[0]);

/*Узнаём наименование вида командировки*/
class iceb_u_str naim_vidkom("");
sprintf(strsql,"select naik from Ukrvkr where kod='%s'",data->vidkom.ravno());


if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
  naim_vidkom.new_plus(row[0]);


sprintf(strsql,"%s %s %s:%s\n",
gettext("Дата документа"),
data->data_dok.ravno(),
gettext("Номер"), 
data->nomd.ravno());

data->hapka.new_plus(strsql);

sprintf(strsql,"%s:%s/%s\n",
gettext("Контрагент"),
data->kontr.ravno(),naim_kontr.ravno());

data->hapka.plus(strsql);


sprintf(strsql,"%s:%s/%s ",
gettext("Вид командировки"),
data->vidkom.ravno(),naim_vidkom.ravno());

data->hapka.plus(strsql);


return(0);
}
/***********************/
/*Удаление записи в документе*/
/****************************/
int l_ukrdok_uz(class ukrdok_data *data)
{

if(iceb_menu_danet(gettext("Удалить запись ? Вы уверены ?"),2,data->window) == 2)
   return(1);    
char strsql[512];
sprintf(strsql,"delete from Ukrdok1 where datd='%s' and nomd='%s' \
and kodr=%s and shet='%s' and nomz=%d",
data->data_dok.ravno_sqldata(),data->nomd.ravno(),data->kodz_v.ravno(),data->shet_zap.ravno(),data->nomzap_v.ravno_atoi());

iceb_sql_zapis(strsql,1,0,data->window);

return(0);
}
/***************************/
/*Удаление всего документа*/
/**************************/
int l_ukrdok_ud(class ukrdok_data *data)
{
char strsql[512];

short dd,md,gd;
iceb_u_rsdat(&dd,&md,&gd,data->data_dok.ravno(),1);



if(iceb_menu_danet(gettext("Удалить документ ? Вы уверены ?"),2,data->window) == 2)
  return(1);    

if(iceb_udprgr(ICEB_MP_UKR,dd,md,gd,data->nomd.ravno(),0,0,data->window) != 0)
 return(1);
 
sprintf(strsql,"delete from Ukrdok1 where datd='%04d-%d-%d' and nomd='%s'",gd,md,dd,data->nomd.ravno());

if(iceb_sql_zapis(strsql,1,0,data->window) != 0)
  return(1);

sprintf(strsql,"delete from Ukrdok where god=%d and nomd='%s'",gd,data->nomd.ravno());

iceb_sql_zapis(strsql,1,0,data->window);

return(0);
}
