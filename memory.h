#ifndef MEMORY_H_
#define MEMORY_H_

GtkWidget *m_label;
GdkPixmap *mem_graph;
GtkWidget *s_label;
GdkPixmap *swap_graph;
GtkWidget *mem_draw_area;   //内存画图区域
GtkWidget *swap_draw_area;  //交换区画图区域
static unsigned m_coory[120];
static unsigned s_coory[120];
float sw_rate;


struct MEM_INFO {
	unsigned long total;
	unsigned long free;
	unsigned long buffers;
	unsigned long cached;
	unsigned long s_total;
	unsigned long s_free;
};
//mem关联函数
static gboolean mem_expose_event(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
	gdk_draw_drawable(widget->window, widget->style->fg_gc[GTK_WIDGET_STATE(widget)], mem_graph, event->area.x, event->area.y, event->area.x, event->area.y, event->area.width, event->area.height);
	return FALSE;
}

static gboolean mem_configure_event(GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
	if (mem_graph)
	{
		g_object_unref(mem_graph);
	}
	mem_graph = gdk_pixmap_new(widget->window, widget->allocation.width, widget->allocation.height, -1);
	gdk_draw_rectangle(mem_graph, widget->style->white_gc, TRUE, 0, 0, widget->allocation.width, widget->allocation.height);
	return TRUE;
}
//swap关联函数
static gboolean swap_expose_event(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
	gdk_draw_drawable(widget->window, widget->style->fg_gc[GTK_WIDGET_STATE(widget)], swap_graph, event->area.x, event->area.y, event->area.x, event->area.y, event->area.width, event->area.height);
	return FALSE;
}

static gboolean swap_configure_event(GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
	if (swap_graph)
	{
		g_object_unref(swap_graph);
	}
	swap_graph = gdk_pixmap_new(widget->window, widget->allocation.width, widget->allocation.height, -1);
	gdk_draw_rectangle(swap_graph, widget->style->white_gc, TRUE, 0, 0, widget->allocation.width, widget->allocation.height);
	return TRUE;
}
//得到内存信息
void get_memstat(struct MEM_INFO *mem)
{
	FILE *fd;
	char str[30];
	char strnum[10];
	unsigned long num = 0;
	fd = fopen("/proc/meminfo", "r");
	if (fd == NULL) {
		printf("open /proc/meminfo error!\n");
		return;
	}
	int i = 0;
	while ((fgets(str, sizeof(str), fd)) != NULL) {
		//printf("%s\n", str);
		int len = strlen(str);
		int j = 0, k = 0;
		for (; j < len; j++) {
			if (str[j] >= '0' && str[j] <= '9') {
				strnum[k++] = str[j];
			}
		}
		strnum[k] = '\0';
		num = strtoul(strnum, NULL, 10);
		if (i == 0) mem->total = num;
		if (i == 1) mem->free = num;
		if (i == 3) mem->buffers = num;
		if (i == 4) mem->cached = num;
		if (i == 18) mem->s_total = num;
		if (i == 19) {
			mem->s_free = num;
			break;
		}

		i++;
	}
	fclose(fd);
}
//计算mem使用率
float get_memrate(struct MEM_INFO *mem)
{
	float memrate = 0.0;
	memrate = (float)((mem->total - (mem->free + mem->cached + mem->buffers)) *100/ mem->total);
	return memrate;
}
//计算swap使用率
float get_swaprate(struct MEM_INFO *mem)
{
	float swaprate = 0.0;
	swaprate = (float)((mem->s_total - mem->s_free) * 100 / mem->s_total);
	return swaprate;
}
void createMemPage(GtkWidget* notebook)
{
	GtkWidget *frame = gtk_frame_new("Memory Page");
	gtk_container_set_border_width(GTK_CONTAINER(frame), 0);
	gtk_widget_set_size_request(frame, 120, 500);
	gtk_widget_show(frame);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), frame, gtk_label_new("Memory and swap area info"));

	//创建一个表格两行两列
	GtkWidget *table = gtk_table_new(2, 2, TRUE);
	gtk_container_add(GTK_CONTAINER(frame), table);
	/*内存信息*/
	//将内存使用率图放到左上方
	GtkWidget *frame_leftup = gtk_frame_new("Memory using img");
	gtk_table_attach_defaults(GTK_TABLE(table), frame_leftup, 0, 1, 0, 1);
	gtk_widget_show(frame_leftup);

	mem_draw_area = gtk_drawing_area_new(); //绘制内存使用率图
	gtk_widget_set_app_paintable(mem_draw_area, TRUE);
	gtk_drawing_area_size(GTK_DRAWING_AREA(mem_draw_area), 40, 40);
	gtk_container_add(GTK_CONTAINER(frame_leftup), mem_draw_area);
	gtk_widget_show(mem_draw_area);
	//关联回调函数
	g_signal_connect(mem_draw_area, "expose_event",G_CALLBACK(mem_expose_event), NULL);
	g_signal_connect(mem_draw_area, "configure_event", G_CALLBACK(mem_configure_event), NULL);
	gtk_widget_show(mem_draw_area);


	/*交换区信息*/
	//将内存使用率图放到右上方
	GtkWidget *frame_rightup = gtk_frame_new("swap area using img"); //绘制交换区使用率图
	gtk_table_attach_defaults(GTK_TABLE(table), frame_rightup, 1, 2, 0, 1);
	gtk_widget_show(frame_rightup);

	swap_draw_area = gtk_drawing_area_new();
	gtk_widget_set_app_paintable(swap_draw_area, TRUE);
	gtk_drawing_area_size(GTK_DRAWING_AREA(swap_draw_area), 40, 40);
	gtk_container_add(GTK_CONTAINER(frame_rightup), swap_draw_area);
	gtk_widget_show(swap_draw_area);
	g_signal_connect(swap_draw_area, "expose_event",G_CALLBACK(swap_expose_event), NULL);
	g_signal_connect(swap_draw_area, "configure_event", G_CALLBACK(swap_configure_event), NULL);
	gtk_widget_show(swap_draw_area);

	GtkWidget *frame_meminfo = gtk_frame_new("Mem Infomation"); //下半部分显示内存相关信息
	gtk_table_attach_defaults(GTK_TABLE(table), frame_meminfo, 0, 1, 1, 2);
	gtk_widget_show(frame_meminfo);

	GtkWidget *vbox = gtk_vbox_new(FALSE, 2);
	gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);
	gtk_container_add(GTK_CONTAINER(frame_meminfo), vbox);
	gtk_widget_show(vbox);
	gtk_widget_show(table);

	struct MEM_INFO Mem_info;
	char Buff[200]="\0", free[50] , buffers[50] , cached[50] , total[50];
	get_memstat(&Mem_info);
	sprintf(total, "Total memory space: %lu KB\n", Mem_info.total);
	sprintf(free, "Free memory space: %lu KB\n", Mem_info.free);
	sprintf(buffers, "block devices space: %lu KB\n", Mem_info.buffers);
	sprintf(cached, "File buffer space: %lu KB\n", Mem_info.cached);
	strcat(Buff, total);
	strcat(Buff, free);
	strcat(Buff, buffers);
	strcat(Buff, cached);
	//设置控件
	GtkWidget *label1 = gtk_label_new(Buff);
	gtk_box_pack_start(GTK_BOX(vbox), label1, FALSE, FALSE, 1);
	gtk_widget_show(label1);
	//交换区信息
	char Buff1[100]="\0" , S_total[50] , S_free[50];
	sprintf(S_total, "Swap memory space: %lu KB\n", Mem_info.s_total);
	sprintf(S_free, "Swap free space: %lu KB\n", Mem_info.s_free);
	strcat(Buff1, S_total);
	strcat(Buff1, S_free);
	GtkWidget *label2 = gtk_label_new(Buff1);
	gtk_box_pack_start(GTK_BOX(vbox), label2, FALSE, FALSE, 1);
	gtk_widget_show(label2);

	//显示利用率
	GtkWidget *frame_rigntdown = gtk_frame_new("using rate"); //下半部分显示内存相关信息
	gtk_table_attach_defaults(GTK_TABLE(table), frame_rigntdown, 1, 2, 1, 2);
	gtk_widget_show(frame_rigntdown);

	GtkWidget *vbox1 = gtk_vbox_new(FALSE, 2);
	gtk_container_set_border_width(GTK_CONTAINER(vbox1), 10);
	gtk_container_add(GTK_CONTAINER(frame_rigntdown), vbox1);
	gtk_widget_show(vbox1);
	gtk_widget_show(table);

	m_label = gtk_label_new("Memory using rate:");
	gtk_box_pack_start(GTK_BOX(vbox1), m_label, FALSE, FALSE, 1);
	gtk_widget_show(m_label);

	char S_buf[40]="\0";
	sprintf(S_buf, "Swap area using rate:\n%.1f", sw_rate);
	strcat(S_buf, " %");
	s_label = gtk_label_new(S_buf);
	gtk_box_pack_start(GTK_BOX(vbox1), s_label, FALSE, FALSE, 1);
	gtk_widget_show(s_label);

}

gint mem_refresh(gpointer data)
{
	/* 建一个矩形绘图区 */
	GtkWidget *drawarea = (GtkWidget *)data;
	GdkDrawable *drawable = drawarea->window;
	GdkGC *gc_chart = gdk_gc_new(drawarea->window);

	/* 背景颜色 */
	GdkColor color;
	color.red = 0x0000;
	color.green = 0x0000;
	color.blue = 0xffff;

	gdk_gc_set_rgb_fg_color(gc_chart, &color);

	int width, height;
	static int flag = 1;
	width = drawarea->allocation.width;
	height = drawarea->allocation.height;
	gdk_draw_rectangle(drawable, drawarea->style->white_gc, TRUE, 0, 0, width, height);

	if (flag) {
		int i;
		for (i = 0; i < 120; i++) m_coory[i] = mem_draw_area->allocation.height;
		flag = 0;
	}

	int x;
	float memrate;
	struct MEM_INFO Mem;
	get_memstat(&Mem);
	memrate = get_memrate(&Mem);
	//输出mem利用率
	char rate[30];

	sprintf(rate, "\n Mem using rate:\n%.1f %\n", memrate);
	gtk_label_set_text(m_label, rate);

	m_coory[119] = (unsigned)(height - memrate / 100 * height);
	for (x = 0; x < 119; x++) m_coory[x] = m_coory[x + 1];
	int step = width / 120;


	for (x = 119; x >= 0; x--) {
		gdk_draw_line(drawable, gc_chart, x*step, m_coory[x], (x - 1)*step, m_coory[x - 1]);
	}
	/* 这是一个 timeout 函数,返回 TRUE,这样它就能够继续被调用 */
	return TRUE;
}
gint swap_refresh(gpointer data)
{
	/* 建一个矩形绘图区 */
	GtkWidget *drawarea = (GtkWidget *)data;
	GdkDrawable *drawable = drawarea->window;
	GdkGC *gc_chart = gdk_gc_new(drawarea->window);

	/* 背景颜色 */
	GdkColor color;
	color.red = 0x0000;
	color.green = 0x0000;
	color.blue = 0xffff;

	gdk_gc_set_rgb_fg_color(gc_chart, &color);

	int width, height;
	static int flag1 = 1;
	width = drawarea->allocation.width;
	height = drawarea->allocation.height;
	gdk_draw_rectangle(drawable, drawarea->style->white_gc, TRUE, 0, 0, width, height);

	if (flag1) {
		int i;
		for (i = 0; i < 120; i++) s_coory[i] = swap_draw_area->allocation.height;
		flag1 = 0;
	}

	int x;
	struct MEM_INFO Swap;
	get_memstat(&Swap);
	sw_rate = get_swaprate(&Swap);
	//输出mem利用率
	char rate[40];

	//建立第二个标签后下面不能用了
	sprintf(rate, "\n Swap area using rate:\n%.1f %\n", sw_rate);
	gtk_label_set_text(s_label, rate);

	s_coory[119] = (unsigned)(height - sw_rate / 100 * height-10);
	for (x = 0; x < 119; x++) s_coory[x] = s_coory[x + 1];
	int step = width / 120;


	for (x = 119; x >= 0; x--) {
		gdk_draw_line(drawable, gc_chart, x*step, s_coory[x], (x - 1)*step, s_coory[x - 1]);
	}
	/* 这是一个 timeout 函数,返回 TRUE,这样它就能够继续被调用 */
	return TRUE;
}
#endif
