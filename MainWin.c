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

	/* 主窗口 */
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "System Moniter");
	gtk_window_set_opacity(GTK_WINDOW(window), 0.95); // 设置透明度函数

	/* 默认窗口大小 */
	gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

	/* 窗口初始位置在屏幕最中央 */
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

	/* 显示窗口 */
	gtk_widget_show(window);

	/* 创建一个纵向盒 */
	main_vbox = gtk_vbox_new(FALSE, 10);
	/* 设定这个容器和周围的间距 */
	gtk_container_set_border_width(GTK_CONTAINER(main_vbox), 5);

	/* 将这个布局容器添加到整个视窗的容器中 */
	gtk_container_add(GTK_CONTAINER(window), main_vbox);

	/* 显示该盒 */
	gtk_widget_show(main_vbox);

	/* 建立多标签页notebook */
	GtkWidget *notebook = gtk_notebook_new();
	gtk_notebook_set_tab_pos((GtkNotebook *)notebook, GTK_POS_BOTTOM);
	gtk_notebook_set_show_border((GtkNotebook *)notebook, FALSE);
	/* 在纵向盒顶部添加该笔记本 */
	gtk_box_pack_end(GTK_BOX(main_vbox), notebook, TRUE, TRUE, 0);

	/* 显示该笔记本 */
	gtk_widget_show(notebook);


	/* 新建第1个标签页到notebook，用的是frame框架 */
	createCPUPage(notebook);
	gtk_timeout_add(400, refresh, (gpointer)cpu_draw_area);  //刷新CPU图

	/* 新建第2个标签页到notebook */
	createProPage(notebook);


	/* 新建第3个标签页到notebook */
	createMemPage(notebook);
	gtk_timeout_add(400, mem_refresh, (gpointer)mem_draw_area);   //刷新内存图
	gtk_timeout_add(400, swap_refresh, (gpointer)swap_draw_area);   //刷新交换区图

	/* 新建第4个标签页到notebook */
	createAboutPage(notebook);
	gtk_timeout_add(400, time_refresh, NULL);   //刷新时间图


	gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), 0);


	gtk_main();
	return 0;
}
