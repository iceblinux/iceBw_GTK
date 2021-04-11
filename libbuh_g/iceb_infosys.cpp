/*$Id: iceb_infosys.c,v 1.31 2013/08/13 06:10:10 sasa Exp $*/
/*22.12.2018	07.12.2000	Белых А.И.	iceb_infosys.c
Вывод на экран системной информации
*/
#include <pwd.h>
#include "iceb_libbuh.h"

extern class iceb_u_str kodirovka_iceb; /*определяется в iceb_start.c iceb_nastsys.c*/

void iceb_infosys(const char *host,const char *Version,
const char *DVERSIQ,short ddd,short mmm,short ggg,
const char *imabaz,iceb_u_str *soob,int kod_operatora)
{
struct  passwd  *ktor; /*Кто работает*/
pid_t	mypid;
char	stroka[112];

if(kod_operatora > 0)
 ktor=getpwuid(kod_operatora);
else
 ktor=getpwuid(getuid());

mypid=getpid();

short dt,mt,gt;
iceb_u_poltekdat(&dt,&mt,&gt);

if(ddd == 0 && mmm == 0 && ggg == 0)
 {
  sprintf(stroka,"%s %d.%d.%d%s",gettext("Текущая дата"),dt,mt,gt,
  gettext("г."));
 }
else
 if(ddd != 0)
   sprintf(stroka,"%s %d.%d.%d%s",gettext("Текущая дата"),ddd,mmm,ggg,
   gettext("г."));
  else
   sprintf(stroka,"%s %d.%d%s",gettext("Текущая дата"),mmm,ggg,
   gettext("г."));

soob->plus_ps(stroka);


sprintf(stroka,"%s%s/%s",gettext("База:"),imabaz,kodirovka_iceb.ravno());
soob->plus_ps(stroka);

sprintf(stroka,"%s%s",gettext("Хост:"),host);

sprintf(stroka,"%s:%d",gettext("Номер процесса"),mypid);
soob->plus_ps(stroka);

sprintf(stroka,"%s:%d %s",gettext("Логин"),ktor->pw_uid,ktor->pw_name);
soob->plus_ps(stroka);

sprintf(stroka,"%s:%-*.*s",gettext("Имя"),
iceb_u_kolbait(20,ktor->pw_gecos),iceb_u_kolbait(20,ktor->pw_gecos),ktor->pw_gecos);
soob->plus_ps(stroka);


sprintf(stroka,"copyright (c) %s 1992-2020",gettext("Белых А.И."));

soob->plus_ps(stroka);

sprintf(stroka,gettext("г. Винница т."));

soob->plus(stroka);
soob->plus_ps("+380676654526");

//sprintf(stroka,"e_mail:sasa@ukrpost.ua");
sprintf(stroka,"http://iceb.net.ua");
soob->plus_ps(stroka);

sprintf(stroka,"%s %s %s %s%s",gettext("Версия"),Version,
gettext("от"),DVERSIQ,
gettext("г."));

soob->plus(stroka);

}
