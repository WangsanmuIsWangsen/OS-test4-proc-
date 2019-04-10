#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <dirent.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <cairo.h>
#include <time.h>

#include "cpu.h"
#include "process.h"
#include "memory.h"
#include "showAbout.h"

int main(int argc, char ** argv)
{
	GtkWidget * window;
	GtkWidget * main_vbox;
	gtk_init(&argc, &argv);

	/* ������ */
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "System Moniter");
	gtk_window_set_opacity(GTK_WINDOW(window), 0.95); // ����͸���Ⱥ���

	/* Ĭ�ϴ��ڴ�С */
	gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

	/* ���ڳ�ʼλ������Ļ������ */
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

	/* ��ʾ���� */
	gtk_widget_show(window);

	/* ����һ������� */
	main_vbox = gtk_vbox_new(FALSE, 10);
	/* �趨�����������Χ�ļ�� */
	gtk_container_set_border_width(GTK_CONTAINER(main_vbox), 5);

	/* ���������������ӵ������Ӵ��������� */
	gtk_container_add(GTK_CONTAINER(window), main_vbox);

	/* ��ʾ�ú� */
	gtk_widget_show(main_vbox);

	/* �������ǩҳnotebook */
	GtkWidget *notebook = gtk_notebook_new();
	gtk_notebook_set_tab_pos((GtkNotebook *)notebook, GTK_POS_BOTTOM);
	gtk_notebook_set_show_border((GtkNotebook *)notebook, FALSE);
	/* ������ж�����ӸñʼǱ� */
	gtk_box_pack_end(GTK_BOX(main_vbox), notebook, TRUE, TRUE, 0);

	/* ��ʾ�ñʼǱ� */
	gtk_widget_show(notebook);


	/* �½���1����ǩҳ��notebook���õ���frame��� */
	createCPUPage(notebook);
	gtk_timeout_add(400, refresh, (gpointer)cpu_draw_area);  //ˢ��CPUͼ

	/* �½���2����ǩҳ��notebook */
	createProPage(notebook);


	/* �½���3����ǩҳ��notebook */
	createMemPage(notebook);
	gtk_timeout_add(400, mem_refresh, (gpointer)mem_draw_area);   //ˢ���ڴ�ͼ
	gtk_timeout_add(400, swap_refresh, (gpointer)swap_draw_area);   //ˢ�½�����ͼ

	/* �½���4����ǩҳ��notebook */
	createAboutPage(notebook);
	gtk_timeout_add(400, time_refresh, NULL);   //ˢ��ʱ��ͼ


	gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), 0);


	gtk_main();
	return 0;
}
