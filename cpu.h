#ifndef CPU_H_
#define CPU_H_
//GtkWidget *window;
GtkWidget *main_vbox;
GtkWidget *cpu_draw_area;   //画图区域
GdkPixmap *cpu_graph;
GtkWidget *pbar_cpu;

/* 全局变量 */
//static gint cpuPoints[100];
//static gfloat cpu_rate = 0.0;
//static char PID[20];
//static int selectedRow;
static unsigned coory[120];
GtkWidget *cpu_label;
typedef struct CPUinfo
{
	char modeName[50]; //line 5
	char cpuMHz[10]; //line 7
	char cpuCores[10]; //line 12
	char cpuUptime[20];
}*cpuInfo;
struct CPU_RATE {
	char name[20];
	unsigned int user;
	unsigned int nice;
	unsigned int system;
	unsigned int idle;
	unsigned int iowait;
	unsigned int irq;
	unsigned int softtirq;
};
struct CPU_RATE lastcpu = { "wangsen",0,0,0,0,0,0,0 };

static gboolean cpu_expose_event(GtkWidget *widget, GdkEventExpose *event,gpointer data)
{
	gdk_draw_drawable(widget->window, widget->style->fg_gc[GTK_WIDGET_STATE(widget)], cpu_graph, event->area.x, event->area.y, event->area.x,event->area.y, event->area.width, event->area.height);
	return FALSE;
}

static gboolean cpu_configure_event(GtkWidget *widget,GdkEventConfigure *event, gpointer data)
{
	if (cpu_graph)
	{
		g_object_unref(cpu_graph);
	}
	cpu_graph = gdk_pixmap_new(widget->window, widget->allocation.width, widget->allocation.height, -1);
	gdk_draw_rectangle(cpu_graph, widget->style->white_gc, TRUE, 0, 0, widget->allocation.width, widget->allocation.height);
	return TRUE;
}
//获得CPU状态
void get_cpu_stat(struct CPU_RATE *cpu)
{
	FILE *fd;
	char buf[256];
	fd = fopen("/proc/stat", "r");
	if (fd == NULL) {
		printf("open /proc/stat error!\n");
		return;
	}
	fgets(buf, sizeof(buf), fd);
	sscanf(buf, "%s %u %u %u %u %u %u %u", cpu->name, &cpu->user, &cpu->nice, &cpu->system, &cpu->idle, &cpu->iowait, &cpu->irq, &cpu->softtirq);
	fclose(fd);
	return;
}

/* 取得cpu利用率 */
float get_cpurate(struct CPU_RATE *cpu1, struct CPU_RATE *cpu2)
{
	unsigned long t1, t2;
	static float lastrate = 0;
	float cpurate, usr_cpurate, kernel_cpurate;
	t1 = cpu1->user + cpu1->system + cpu1->nice + cpu1->idle;
	t2 = cpu2->user + cpu2->system + cpu2->nice + cpu2->idle;
	if (t1 == t2) {
		cpurate = lastrate;
	}
	else {
		usr_cpurate = (cpu2->user - cpu1->user) * 100 / (t2 - t1);
		kernel_cpurate = (cpu2->system - cpu1->system) * 100 / (t2 - t1);
		cpurate = usr_cpurate + kernel_cpurate;
		lastrate = cpurate;
	}
	return cpurate;
}
//get info
void getInforNotd(char *path, char *name, char infor[])
{

	int fd = open(path, O_RDONLY);
	char store[2000];
	int k = 0;
	char *p = NULL;
	read(fd, store, sizeof(store));
	close(fd);
	/* 取得所需要信息所在行首尾指针 */
	p = strstr(store, name);

	/* 跳过标题 */
	do
	{
		p++;
	} while (*p != ':');
	p += 2;
	/* 读取数值 */
	while (*p != '\n')
	{
		infor[k] = *p;
		k++;
		p++;
	}
	infor[k] = '\0';

}
//get sys version
void getSysVersion(char *path, char *name, char infor[])
{
	int fd = open(path, O_RDONLY);
	char store[2000];
	int k = 0;
	char *p = NULL;
	read(fd, store, sizeof(store));
	close(fd);
	/* 取得所需要信息所在行首尾指针 */
	p = strstr(store, name);

	/* 读取数值 */
	while (*p != '\n' && *p != ')')
	{
		infor[k] = *p;
		k++;
		p++;
	}

	infor[k++] = ')';
	infor[k] = '\0';

}
/* cpu标签页，显示cpu信息 */
void createCPUPage(GtkWidget* notebook)
{
	GtkWidget *label;
	GtkWidget *vbox;
	GtkWidget *frame1 = gtk_frame_new("CPU Page");
	gtk_container_set_border_width(GTK_CONTAINER(frame1), 10);
	gtk_widget_set_size_request(frame1, 120, 500);
	gtk_widget_show(frame1);

	/* 将该标签页加入到notebook中 */
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), frame1, gtk_label_new("CPU info"));

	/* 在该页建1个表格，分为上下2行，一列 */
	GtkWidget *table1 = gtk_table_new(2, 2, TRUE);
	/* 将这个表添加到“cpu使用率“页 */
	gtk_container_add(GTK_CONTAINER(frame1), table1);
	/* 新建一个frame框架用于显示图表 */
	GtkWidget *CPU_frame1 = gtk_frame_new("CPU curve");
	/* 放到表格第一行 */
	gtk_table_attach_defaults(GTK_TABLE(table1), CPU_frame1, 0, 2, 0, 1);
	gtk_widget_show(CPU_frame1);

	/* 屏幕绘图。
	 * 我们使用的构件是绘图区构件。
	 *一个绘图区构件本质上是一个 X 窗口,没有其它的东西。
	 *它是一个空白的画布,我们可以在其上绘制需要的东西。*/
	cpu_draw_area = gtk_drawing_area_new();

	/* 设置可以画图 */
	gtk_widget_set_app_paintable(cpu_draw_area, TRUE);

	/* 画图区默认大小 */
	gtk_drawing_area_size(GTK_DRAWING_AREA(cpu_draw_area), 40, 40);
	/* 画布添加到cpu曲线框架中 */
	gtk_container_add(GTK_CONTAINER(CPU_frame1), cpu_draw_area);
	gtk_widget_show(cpu_draw_area);
	/* 关联回调函数 */
	g_signal_connect(cpu_draw_area, "expose_event",G_CALLBACK(cpu_expose_event), NULL);
	g_signal_connect(cpu_draw_area, "configure_event", G_CALLBACK(cpu_configure_event), NULL);
	gtk_widget_show(cpu_draw_area);


	/* 取得CPU信息 */
	char modeName[50], cpuMHz[20], cpuCores[20];
	char cpuBuffer[1000];

	//得到CPU的内容
	getInforNotd("/proc/cpuinfo", "model name", modeName);
	getInforNotd("/proc/cpuinfo", "cpu MHz", cpuMHz);
	getInforNotd("/proc/cpuinfo", "cpu cores", cpuCores);



	/* 建一个frame在表一下半部显示cpu信息 */
	GtkWidget *frame_down = gtk_frame_new("system info ");
	gtk_table_attach_defaults(GTK_TABLE(table1), frame_down, 0, 2, 1, 2);
	gtk_widget_show(frame_down);

	gtk_widget_show(table1);

	/* 在该框架建一个表格，将下半部分为1行，2列 */
	GtkWidget *table_down = gtk_table_new(1, 2, TRUE);
	/* 将这个表添加到下面框架 */
	gtk_container_add(GTK_CONTAINER(frame_down), table_down);

	/* 建一个frame在表二左半部显示硬件信息 */
	GtkWidget *frame_left_down = gtk_frame_new("hardware");
	gtk_table_attach_defaults(GTK_TABLE(table_down), frame_left_down, 0, 1, 0, 1);

	gtk_widget_show(frame_left_down);


	gtk_widget_show(table_down);

	/* 容器 */
	vbox = gtk_vbox_new(FALSE, 3);
	gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);
	gtk_container_add(GTK_CONTAINER(frame_left_down), vbox);
	gtk_widget_show(vbox);
	/* 建一个标签显示cpu型号 */
	strcpy(cpuBuffer, "CPU model and frequency:\n");
	strcat(cpuBuffer, modeName);

	/* 建一个标签显示cpu主频 */
	strcat(cpuBuffer, "\n\ncpu frequency:");
	strcat(cpuBuffer, cpuMHz);
	strcat(cpuBuffer, " MHz");
	/* 建一个标签显示cpu核数 */
	strcat(cpuBuffer, "\n\ncpu number:");
	strcat(cpuBuffer, cpuCores);

	label = gtk_label_new(cpuBuffer);
	gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 5);
	gtk_widget_show(label);

	//显示CPU使用率
	cpu_label = gtk_label_new("\nCPU using rate:\n");
	gtk_box_pack_start(GTK_BOX(vbox), cpu_label, FALSE, FALSE, 5);
	gtk_widget_show(cpu_label);

	/* 建一个frame在表二右半部显示系统版本信号 */
	GtkWidget *frame_right_down = gtk_frame_new("Ubuntu");
	gtk_table_attach_defaults(GTK_TABLE(table_down), frame_right_down, 1, 2, 0, 1);

	gtk_widget_show(frame_right_down);


	gtk_widget_show(table_down);

	/* 容器 */
	vbox = gtk_vbox_new(FALSE, 5);
	gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);
	gtk_container_add(GTK_CONTAINER(frame_right_down), vbox);
	gtk_widget_show(vbox);
	/* 建一个标签显示cpu型号 */
	char sysversion[50];
	memset(cpuBuffer, 0, 1000);
	memset(sysversion, 0, 50);
	getSysVersion("/proc/version", "version", sysversion);
	strcat(cpuBuffer, "\n\nkernel version : ");
	strcat(cpuBuffer, sysversion);
	strcat(cpuBuffer, "\n\nsystem version 10.10 (maverick)");
	strcat(cpuBuffer, "\n\nGNOME 2.32.0");
	GtkWidget *label1 = gtk_label_new(cpuBuffer);
	gtk_box_pack_start(GTK_BOX(vbox), label1, FALSE, FALSE, 5);
	gtk_widget_show(label1);


}
//画图函数
gint refresh(gpointer data)
{
	/* cpu */
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
		for (i = 0; i < 120; i++) coory[i] = cpu_draw_area->allocation.height;
		flag = 0;
	}

	int x;
	float cpurate;
	struct CPU_RATE cpu;
	get_cpu_stat(&cpu);
	cpurate = get_cpurate(&lastcpu, &cpu);
	lastcpu = cpu;
	//printf("%d\n", cpurate);
	//输出cpu利用率
	char rate[30];

	sprintf(rate, "\nCPU using rate:\n%.1f %\n", cpurate);
	gtk_label_set_text(cpu_label, rate);

	coory[119] = (unsigned)(height - cpurate / 50 * height);
	for (x = 0; x < 119; x++) coory[x] = coory[x + 1];
	int step = width / 120;


	for (x = 119; x >= 0; x--) {
		gdk_draw_line(drawable, gc_chart, x*step, coory[x], (x - 1)*step, coory[x - 1]);
	}
	/* 这是一个 timeout 函数,返回 TRUE,这样它就能够继续被调用 */
	return TRUE;
}

#endif /* CPU_H_ */
