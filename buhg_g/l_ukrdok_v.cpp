/*$Id: l_ukrdok_v.c,v 1.18 2013/08/25 08:26:39 sasa Exp $*/
/*12.07.2015	21.02.2008	Белых А.И.	l_ukrdok_v.c
Ввод и корректировка записи в документ
*/
#include "buhg_g.h"

enum
{
  FK2,
  FK4,
  FK10,
  KOL_F_KL
};

enum
 {
  E_KOLIH,
  E_SUMA_S_NDS,
  E_SUMA_NDS,
  E_EI,
  E_SHET,
  E_KONTR_F,
  E_KONTR_R,
  E_NOMDOK,
  E_SHET_NDS,
  E_DVNN,
  E_STRAH_SBOR,
  KOLENTER  
 };

class l_ukrdok_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  GtkWidget *label_naim[KOLENTER];
  short kl_shift;
  int voz;  
  
  short dd,md,gd;  
  class iceb_u_str nom_dok;
  int kodk; /*Код записи которую корректируем*/
  int nomzap; /*Номер записи в документе*/
  class iceb_u_str shet_zap; /*Счёт в корректируемой записи*/
  
  class iceb_u_str kolih;
  class iceb_u_str suma_s_nds;
  class iceb_u_str suma_nds;
  class iceb_u_str ei;
  class iceb_u_str shet;
  class iceb_u_str kontr_f;
  class iceb_u_str kontr_r;
  class iceb_u_str nomdok;
  class iceb_u_str shet_nds;
  class iceb_u_str dvnn;  
  class iceb_u_str strah_sbor;
  
  l_ukrdok_v_data() //Конструктор
   {
    voz=0;
    kl_shift=0;
    clear();
  
   }
  void clear()
   {
    kolih.new_plus("");
    suma_s_nds.new_plus("");
    suma_nds.new_plus("");
    ei.new_plus("");
    shet.new_plus("");
    kontr_f.new_plus("");
    kontr_r.new_plus("");
    nomdok.new_plus("");
    shet_nds.new_plus("");
    dvnn.new_plus("");
    strah_sbor.new_plus("");
   }
  void read_rek()
   {
    kontr_f.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KONTR_F])));
    kontr_r.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KONTR_R])));
    kolih.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOLIH])));
    suma_s_nds.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_SUMA_S_NDS])));
    suma_nds.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_SUMA_NDS])));
    shet.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_SHET])));
    shet_nds.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_SHET_NDS])));
    ei.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_EI])));
    nomdok.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NOMDOK])));
    dvnn.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DVNN])));
    strah_sbor.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_STRAH_SBOR])));
   }
  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
    clear();
    gtk_label_set_text(GTK_LABEL(label_naim[E_SHET]),"");
    gtk_label_set_text(GTK_LABEL(label_naim[E_SHET_NDS]),"");
    gtk_label_set_text(GTK_LABEL(label_naim[E_KONTR_F]),"");
    gtk_label_set_text(GTK_LABEL(label_naim[E_KONTR_R]),"");
    gtk_label_set_text(GTK_LABEL(label_naim[E_EI]),"");

   }
 };

gboolean   l_ukrdok_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_ukrdok_v_data *data);
void    l_ukrdok_v_vvod(GtkWidget *widget,class l_ukrdok_v_data *data);
void  l_ukrdok_v_knopka(GtkWidget *widget,class l_ukrdok_v_data *data);
void  l_ukrdok_v_e_knopka(GtkWidget *widget,class l_ukrdok_v_data *data);

int l_ukrdok_v_zap(class l_ukrdok_v_data *data);

extern SQL_baza bd;

int l_ukrdok_v(short dd,short md,short gd,
const char *nomdok,
const char *shet_zap,
int kod,int nomzap,GtkWidget *wpredok)
{
class l_ukrdok_v_data data;
data.dd=dd;
data.md=md;
data.gd=gd;
data.nom_dok.new_plus(nomdok);
data.kodk=kod;
data.nomzap=nomzap;
data.shet_zap.new_plus(shet_zap);
class iceb_u_str n_kontr_f("");
class iceb_u_str n_kontr_r("");
class iceb_u_str n_shet("");
class iceb_u_str n_shet_nds("");
class iceb_u_str n_ei("");


char strsql[512];

if(data.shet_zap.getdlinna() > 1) /*корректировка существующей записи*/
 {
  SQL_str row;
  class SQLCURSOR cur;
  
  sprintf(strsql,"select * from Ukrdok1 where datd='%04d-%02d-%02d' \
and nomd='%s' and kodr=%d and shet='%s' and nomz=%d",
  data.gd,data.md,data.dd,data.nom_dok.ravno(),data.kodk,data.shet_zap.ravno(),data.nomzap);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
   {
    iceb_menu_soob(gettext("Не найдена запись для корректировки!"),wpredok);
    return(1);
   }  
  data.shet.new_plus(row[3]);
  data.kolih.new_plus(row[4]);
  
  double suma_bez_nds=atof(row[5]);
  data.ei.new_plus(row[6]);
  double suma_nds=atof(row[7]);
  data.kontr_f.new_plus(row[11]);
  data.kontr_r.new_plus(row[12]);
  data.nomdok.new_plus(row[13]);
  data.shet_nds.new_plus(row[14]);
  data.suma_s_nds.new_plus(suma_bez_nds+suma_nds+atof(row[16]));
  data.suma_nds.new_plus(suma_nds);
  data.dvnn.new_plus(iceb_u_datzap(row[15]));
  data.strah_sbor.new_plus(row[16]);
    
  /*Узнаём наименование единицы измерения*/
  if(data.ei.getdlinna() > 1)
   {
    sprintf(strsql,"select naik from Edizmer where kod='%s'",data.ei.ravno());
    if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
     n_ei.new_plus(row[0]);
   }
  /*узнаём наименование контрагента - физическое лицо*/
  if(data.kontr_f.getdlinna() > 1)
   {
    sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",data.kontr_f.ravno());
    if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
     n_kontr_f.new_plus(row[0]);
   }
  /*узнаём наименование контрагента - для реестра*/
  if(data.kontr_r.getdlinna() > 1)
   {
    sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",data.kontr_r.ravno());
    if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
     n_kontr_r.new_plus(row[0]);
   }
  /*узнаём наименование счёта*/
  if(data.shet.getdlinna() > 1)
   {
    sprintf(strsql,"select nais from Plansh where ns='%s'",data.shet.ravno());
    if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
     n_shet.new_plus(row[0]);
   }
  /*узнаём наименование счёта ндс*/
  if(data.shet_nds.getdlinna() > 1)
   {
    sprintf(strsql,"select nais from Plansh where ns='%s'",data.shet_nds.ravno());
    if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
     n_shet_nds.new_plus(row[0]);
   }
  
 }


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(data.shet_zap.getdlinna() > 1)
  sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Корректировка"));
else
  sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Ввод новой записи"));

gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_ukrdok_v_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }


GtkWidget *label=NULL;

if(data.shet_zap.getdlinna() > 1)
  label=gtk_label_new(gettext("Корректировка"));
else
  label=gtk_label_new(gettext("Ввод новой записи"));

GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL,1);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
 {
  hbox[i] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL,1);
gtk_box_set_homogeneous (GTK_BOX(  hbox[i]),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
 }

GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL,1);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);

for(int i=0; i < KOLENTER; i++)
  gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);

gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


sprintf(strsql,"%s",gettext("Количество"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOLIH]), label, FALSE, FALSE,1);

data.entry[E_KOLIH] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOLIH]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_KOLIH]), data.entry[E_KOLIH], TRUE, TRUE,1);
g_signal_connect(data.entry[E_KOLIH], "activate",G_CALLBACK(l_ukrdok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOLIH]),data.kolih.ravno());
gtk_widget_set_name(data.entry[E_KOLIH],iceb_u_inttochar(E_KOLIH));


sprintf(strsql,"%s",gettext("Сумма с НДС"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA_S_NDS]), label, FALSE, FALSE,1);

data.entry[E_SUMA_S_NDS] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SUMA_S_NDS]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA_S_NDS]), data.entry[E_SUMA_S_NDS], TRUE, TRUE,1);
g_signal_connect(data.entry[E_SUMA_S_NDS], "activate",G_CALLBACK(l_ukrdok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SUMA_S_NDS]),data.suma_s_nds.ravno());
gtk_widget_set_name(data.entry[E_SUMA_S_NDS],iceb_u_inttochar(E_SUMA_S_NDS));


sprintf(strsql,"%s",gettext("Сумма НДС"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA_NDS]), label, FALSE, FALSE,1);

data.entry[E_SUMA_NDS] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SUMA_NDS]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA_NDS]), data.entry[E_SUMA_NDS], TRUE, TRUE,1);
g_signal_connect(data.entry[E_SUMA_NDS], "activate",G_CALLBACK(l_ukrdok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SUMA_NDS]),data.suma_nds.ravno());
gtk_widget_set_name(data.entry[E_SUMA_NDS],iceb_u_inttochar(E_SUMA_NDS));


sprintf(strsql,"%s",gettext("Единица измерения"));
data.knopka_enter[E_EI]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_EI]), data.knopka_enter[E_EI], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_EI],"clicked",G_CALLBACK(l_ukrdok_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_EI],iceb_u_inttochar(E_EI));
gtk_widget_set_tooltip_text(data.knopka_enter[E_EI],gettext("Выбор единицы измерения"));

data.entry[E_EI] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_EI]),9);
gtk_box_pack_start (GTK_BOX (hbox[E_EI]), data.entry[E_EI], TRUE, TRUE,1);
g_signal_connect(data.entry[E_EI], "activate",G_CALLBACK(l_ukrdok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_EI]),data.ei.ravno());
gtk_widget_set_name(data.entry[E_EI],iceb_u_inttochar(E_EI));

data.label_naim[E_EI]=gtk_label_new(n_ei.ravno(20));
gtk_box_pack_start (GTK_BOX (hbox[E_EI]), data.label_naim[E_EI], TRUE, TRUE,1);


sprintf(strsql,"%s",gettext("Счёт"));
data.knopka_enter[E_SHET]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.knopka_enter[E_SHET], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_SHET],"clicked",G_CALLBACK(l_ukrdok_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_SHET],iceb_u_inttochar(E_SHET));
gtk_widget_set_tooltip_text(data.knopka_enter[E_SHET],gettext("Выбор счёта в плане счетов"));

data.entry[E_SHET] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SHET]),19);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.entry[E_SHET], TRUE, TRUE,1);
g_signal_connect(data.entry[E_SHET], "activate",G_CALLBACK(l_ukrdok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET]),data.shet.ravno());
gtk_widget_set_name(data.entry[E_SHET],iceb_u_inttochar(E_SHET));

data.label_naim[E_SHET]=gtk_label_new(n_shet.ravno(20));
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.label_naim[E_SHET], TRUE, TRUE,1);



sprintf(strsql,"%s",gettext("Код контрагента (физическое лицо)"));
data.knopka_enter[E_KONTR_F]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KONTR_F]), data.knopka_enter[E_KONTR_F], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_KONTR_F],"clicked",G_CALLBACK(l_ukrdok_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KONTR_F],iceb_u_inttochar(E_KONTR_F));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KONTR_F],gettext("Выбор контрагента"));

data.entry[E_KONTR_F] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KONTR_F]),19);
gtk_box_pack_start (GTK_BOX (hbox[E_KONTR_F]), data.entry[E_KONTR_F], TRUE, TRUE,1);
g_signal_connect(data.entry[E_KONTR_F], "activate",G_CALLBACK(l_ukrdok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KONTR_F]),data.kontr_f.ravno());
gtk_widget_set_name(data.entry[E_KONTR_F],iceb_u_inttochar(E_KONTR_F));

data.label_naim[E_KONTR_F]=gtk_label_new(n_kontr_f.ravno(20));
gtk_box_pack_start (GTK_BOX (hbox[E_KONTR_F]), data.label_naim[E_KONTR_F], TRUE, TRUE,1);


sprintf(strsql,"%s",gettext("Код контрагента для реестра налоговых накладных"));
data.knopka_enter[E_KONTR_R]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KONTR_R]), data.knopka_enter[E_KONTR_R], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_KONTR_R],"clicked",G_CALLBACK(l_ukrdok_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KONTR_R],iceb_u_inttochar(E_KONTR_R));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KONTR_R],gettext("Выбор контрагента"));

data.entry[E_KONTR_R] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KONTR_R]),19);
gtk_box_pack_start (GTK_BOX (hbox[E_KONTR_R]), data.entry[E_KONTR_R], TRUE, TRUE,1);
g_signal_connect(data.entry[E_KONTR_R], "activate",G_CALLBACK(l_ukrdok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KONTR_R]),data.kontr_r.ravno());
gtk_widget_set_name(data.entry[E_KONTR_R],iceb_u_inttochar(E_KONTR_R));

data.label_naim[E_KONTR_R]=gtk_label_new(n_kontr_r.ravno(20));
gtk_box_pack_start (GTK_BOX (hbox[E_KONTR_R]), data.label_naim[E_KONTR_R], TRUE, TRUE,1);


sprintf(strsql,"%s",gettext("Счёт НДС"));
data.knopka_enter[E_SHET_NDS]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET_NDS]), data.knopka_enter[E_SHET_NDS], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_SHET_NDS],"clicked",G_CALLBACK(l_ukrdok_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_SHET_NDS],iceb_u_inttochar(E_SHET_NDS));
gtk_widget_set_tooltip_text(data.knopka_enter[E_SHET_NDS],gettext("Выбор счёта в плане счетов"));

data.entry[E_SHET_NDS] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SHET_NDS]),19);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET_NDS]), data.entry[E_SHET_NDS], TRUE, TRUE,1);
g_signal_connect(data.entry[E_SHET_NDS], "activate",G_CALLBACK(l_ukrdok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET_NDS]),data.shet_nds.ravno());
gtk_widget_set_name(data.entry[E_SHET_NDS],iceb_u_inttochar(E_SHET_NDS));

data.label_naim[E_SHET_NDS]=gtk_label_new(n_shet_nds.ravno(20));
gtk_box_pack_start (GTK_BOX (hbox[E_SHET_NDS]), data.label_naim[E_SHET_NDS], TRUE, TRUE,1);


sprintf(strsql,"%s",gettext("Номер документа"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), label, FALSE, FALSE,1);

data.entry[E_NOMDOK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NOMDOK]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), data.entry[E_NOMDOK], TRUE, TRUE,1);
g_signal_connect(data.entry[E_NOMDOK], "activate",G_CALLBACK(l_ukrdok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMDOK]),data.nomdok.ravno());
gtk_widget_set_name(data.entry[E_NOMDOK],iceb_u_inttochar(E_NOMDOK));



sprintf(strsql,"%s (%s)",gettext("Дата виписки налоговой накладной"),gettext("д.м.г"));
data.knopka_enter[E_DVNN]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DVNN]), data.knopka_enter[E_DVNN], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_DVNN],"clicked",G_CALLBACK(l_ukrdok_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DVNN],iceb_u_inttochar(E_DVNN));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DVNN],gettext("Выбор даты"));

data.entry[E_DVNN] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DVNN]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DVNN]), data.entry[E_DVNN], TRUE, TRUE,1);
g_signal_connect(data.entry[E_DVNN], "activate",G_CALLBACK(l_ukrdok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DVNN]),data.dvnn.ravno());
gtk_widget_set_name(data.entry[E_DVNN],iceb_u_inttochar(E_DVNN));

sprintf(strsql,"%s",gettext("Стаховой сбор"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_STRAH_SBOR]), label, FALSE, FALSE,1);

data.entry[E_STRAH_SBOR] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_STRAH_SBOR]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_STRAH_SBOR]), data.entry[E_STRAH_SBOR], TRUE, TRUE,1);
g_signal_connect(data.entry[E_STRAH_SBOR], "activate",G_CALLBACK(l_ukrdok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_STRAH_SBOR]),data.strah_sbor.ravno());
gtk_widget_set_name(data.entry[E_STRAH_SBOR],iceb_u_inttochar(E_STRAH_SBOR));


sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Запись введённой в меню информации"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(l_ukrdok_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE,1);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введенной информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(l_ukrdok_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE,1);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(l_ukrdok_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE,1);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();


if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);

}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  l_ukrdok_v_e_knopka(GtkWidget *widget,class l_ukrdok_v_data *data)
{
iceb_u_str kod("");
iceb_u_str naim("");

int knop=atoi(gtk_widget_get_name(widget));
//g_print("ukrspdok_v_e_knopka knop=%d\n",knop);

switch (knop)
 {

  case E_KONTR_F:

    if(iceb_l_kontr(1,&kod,&naim,data->window) == 0)
     {
      data->kontr_f.new_plus(kod.ravno());
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_KONTR_F]),data->kontr_f.ravno());
      gtk_label_set_text(GTK_LABEL(data->label_naim[E_KONTR_F]),naim.ravno(20));
     }
    return;  

  case E_KONTR_R:
    if(iceb_l_kontr(1,&kod,&naim,data->window) == 0)
     {
      data->kontr_r.new_plus(kod.ravno());
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_KONTR_R]),data->kontr_r.ravno());
      gtk_label_set_text(GTK_LABEL(data->label_naim[E_KONTR_R]),naim.ravno(20));
     }

    return;  


  case E_SHET:

    if(iceb_l_plansh(1,&kod,&naim,data->window) == 0)
     {
      data->shet.new_plus(kod.ravno());
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET]),data->shet.ravno());
      gtk_label_set_text(GTK_LABEL(data->label_naim[E_SHET]),naim.ravno(20));
     }

    return;  

  case E_DVNN:
    if(iceb_calendar(&data->dvnn,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DVNN]),data->dvnn.ravno());


    return;  


  case E_SHET_NDS:

    if(iceb_l_plansh(1,&kod,&naim,data->window) == 0)
     {
      data->shet_nds.new_plus(kod.ravno());
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET_NDS]),data->shet_nds.ravno());
      gtk_label_set_text(GTK_LABEL(data->label_naim[E_SHET_NDS]),naim.ravno(20));
     }

    return;  

  case E_EI:

    if(iceb_l_ei(1,&kod,&naim,data->window) == 0)
     {
      data->ei.new_plus(kod.ravno());
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_EI]),data->ei.ravno());
      gtk_label_set_text(GTK_LABEL(data->label_naim[E_EI]),naim.ravno(20));
     }

    return;  
    
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_ukrdok_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_ukrdok_v_data *data)
{

//printf("l_ukrdok_v_key_press\n");
switch(event->keyval)
 {

  case GDK_KEY_F2:
    g_signal_emit_by_name(data->knopka[FK2],"clicked");
    return(TRUE);

  case GDK_KEY_F4:
    g_signal_emit_by_name(data->knopka[FK4],"clicked");
    return(TRUE);

    
  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka[FK10],"clicked");
    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
//    printf("Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);
 }

return(TRUE);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  l_ukrdok_v_knopka(GtkWidget *widget,class l_ukrdok_v_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню
    
    if(l_ukrdok_v_zap(data) != 0)
     return;

    data->voz=0;
    gtk_widget_destroy(data->window);
    return;  


  case FK4:
    data->clear_rek();
    return;
    
  case FK10:
    data->read_rek(); //Читаем реквизиты меню
    data->voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    l_ukrdok_v_vvod(GtkWidget *widget,class l_ukrdok_v_data *data)
{
//short d,m,g;

int enter=atoi(gtk_widget_get_name(widget));
//g_print("l_ukrdok_v_vvod enter=%d\n",enter);
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
class iceb_u_str naim("");
switch (enter)
 {

  case E_KONTR_F:
    data->kontr_f.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",data->kontr_f.ravno());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
     naim.new_plus(row[0]);     
    gtk_label_set_text(GTK_LABEL(data->label_naim[E_KONTR_F]),naim.ravno(20));
    break;

  case E_KONTR_R:
    data->kontr_r.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",data->kontr_r.ravno());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
     naim.new_plus(row[0]);     
    gtk_label_set_text(GTK_LABEL(data->label_naim[E_KONTR_R]),naim.ravno(20));
    break;

  case E_KOLIH:
    data->kolih.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;


  case E_SUMA_S_NDS:
    data->suma_s_nds.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_SUMA_NDS:
    data->suma_nds.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_SHET:
    data->shet.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));

    sprintf(strsql,"select nais from Plansh where ns='%s'",data->shet.ravno());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
     naim.new_plus(row[0]);     
    gtk_label_set_text(GTK_LABEL(data->label_naim[E_SHET]),naim.ravno(20));

    break;

  case E_SHET_NDS:
    data->shet_nds.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    sprintf(strsql,"select nais from Plansh where ns='%s'",data->shet_nds.ravno());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
     naim.new_plus(row[0]);     
    gtk_label_set_text(GTK_LABEL(data->label_naim[E_SHET_NDS]),naim.ravno(20));
    break;


  case E_EI:
    data->ei.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));

    sprintf(strsql,"select naik from Edizmer where kod='%s'",data->ei.ravno());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
     naim.new_plus(row[0]);     

    gtk_label_set_text(GTK_LABEL(data->label_naim[E_EI]),naim.ravno(20));

    break;

  case E_NOMDOK:
    data->nomdok.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;


 }

enter+=1;

if(enter >= KOLENTER)
enter=0;


gtk_widget_grab_focus(data->entry[enter]);
 
}
/**************************************/
/*Запись*/
/**************************************/
int l_ukrdok_v_zap(class l_ukrdok_v_data *data)
{
char strsql[512];

if(l_ukrdok_pb(data->md,data->gd,data->window) != 0)
 return(1);


if(data->kolih.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не ввели количество"),data->window);
  return(1);
 }
if(data->suma_s_nds.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не ввели сумму с НДС!"),data->window);
  return(1);
 }
if(data->suma_nds.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не ввели сумму НДС!"),data->window);
  return(1);
 }
if(data->ei.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не ввели единицу измерения!"),data->window);
  return(1);
 }
if(data->shet.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не ввели счёт!"),data->window);
  return(1);
 }

OPSHET shetv;
if(iceb_prsh1(data->shet.ravno(),&shetv,data->window) != 0)
 return(1);

if(data->shet_nds.getdlinna() > 1)
  if(iceb_prsh1(data->shet_nds.ravno(),&shetv,data->window) != 0)
   return(1);
if(data->kontr_r.getdlinna() > 1)
 {
  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",data->kontr_r.ravno());
  if(iceb_sql_readkey(strsql,data->window) != 1)
   {
    iceb_menu_soob(gettext("Не найден код контрагента для налоговых накладных!"),data->window);
    return(1);
   }
 }
if(data->kontr_f.getdlinna() > 1)
 {
  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",data->kontr_f.ravno());
  if(iceb_sql_readkey(strsql,data->window) != 1)
   {
    iceb_menu_soob(gettext("Не найден код контрагента для физического лица!"),data->window);
    return(1);
   }
 }
if(data->dvnn.prov_dat() != 0)
 {
  iceb_menu_soob(gettext("Неправильно ввели дату выписки!"),data->window);
  return(1);
 }
 
if(data->shet.getdlinna() > 1) /*Корректировка*/
 {

  sprintf(strsql,"delete from Ukrdok1 where \
datd='%04d-%d-%d' and nomd='%s' and kodr=%d and shet='%s' and nomz=%d",
  data->gd,data->md,data->dd,data->nom_dok.ravno(),data->kodk,data->shet_zap.ravno(),data->nomzap);
 
  iceb_sql_zapis(strsql,1,0,data->window);

 }
 
time_t vrem;
time(&vrem);
int nomz=0;

snova:;
           
sprintf(strsql,"insert into Ukrdok1 values \
('%04d-%d-%d','%s',%d,'%s',%.10g,%.10g,'%s',%.2f,%d,%ld,%d,'%s','%s','%s','%s','%s',%.2f)",
data->gd,data->md,data->dd,data->nom_dok.ravno(),data->kodk,data->shet.ravno(),data->kolih.ravno_atof(),
data->suma_s_nds.ravno_atof() - data->suma_nds.ravno_atof()- data->strah_sbor.ravno_atof(),
data->ei.ravno_filtr(),
data->suma_nds.ravno_atof(),
iceb_getuid(data->window),
vrem,
nomz,
data->kontr_f.ravno(),
data->kontr_r.ravno(),
data->nomdok.ravno_filtr(),
data->shet_nds.ravno_filtr(),
data->dvnn.ravno_sqldata(),
data->strah_sbor.ravno_atof());

if(sql_zap(&bd,strsql) != 0)
 {
   if(sql_nerror(&bd) == ER_DUP_ENTRY) //Запись уже есть
    {
     nomz++;
     goto snova;
    }
   else
    {
     iceb_msql_error(&bd,gettext("Ошибка записи !"),strsql,data->window);
     return(1);
    }
 }

return(0);
}
