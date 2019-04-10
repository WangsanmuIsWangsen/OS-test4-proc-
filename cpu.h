#ifndef CPU_H_
#define CPU_H_
//GtkWidget *window;
GtkWidget *main_vbox;
GtkWidget *cpu_draw_area;   //��ͼ����
GdkPixmap *cpu_graph;
GtkWidget *pbar_cpu;

/* ȫ�ֱ��� */
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
//���CPU״̬
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

/* ȡ��cpu������ */
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
	/* ȡ������Ҫ��Ϣ��������βָ�� */
	p = strstr(store, name);

	/* �������� */
	do
	{
		p++;
	} while (*p != ':');
	p += 2;
	/* ��ȡ��ֵ */
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
	/* ȡ������Ҫ��Ϣ��������βָ�� */
	p = strstr(store, name);

	/* ��ȡ��ֵ */
	while (*p != '\n' && *p != ')')
	{
		infor[k] = *p;
		k++;
		p++;
	}

	infor[k++] = ')';
	infor[k] = '\0';

}
/* cpu��ǩҳ����ʾcpu��Ϣ */
void createCPUPage(GtkWidget* notebook)
{
	GtkWidget *label;
	GtkWidget *vbox;
	GtkWidget *frame1 = gtk_frame_new("CPU Page");
	gtk_container_set_border_width(GTK_CONTAINER(frame1), 10);
	gtk_widget_set_size_request(frame1, 120, 500);
	gtk_widget_show(frame1);

	/* ���ñ�ǩҳ���뵽notebook�� */
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), frame1, gtk_label_new("CPU info"));

	/* �ڸ�ҳ��1����񣬷�Ϊ����2�У�һ�� */
	GtkWidget *table1 = gtk_table_new(2, 2, TRUE);
	/* ���������ӵ���cpuʹ���ʡ�ҳ */
	gtk_container_add(GTK_CONTAINER(frame1), table1);
	/* �½�һ��frame���������ʾͼ�� */
	GtkWidget *CPU_frame1 = gtk_frame_new("CPU curve");
	/* �ŵ�����һ�� */
	gtk_table_attach_defaults(GTK_TABLE(table1), CPU_frame1, 0, 2, 0, 1);
	gtk_widget_show(CPU_frame1);

	/* ��Ļ��ͼ��
	 * ����ʹ�õĹ����ǻ�ͼ��������
	 *һ����ͼ��������������һ�� X ����,û�������Ķ�����
	 *����һ���հ׵Ļ���,���ǿ��������ϻ�����Ҫ�Ķ�����*/
	cpu_draw_area = gtk_drawing_area_new();

	/* ���ÿ��Ի�ͼ */
	gtk_widget_set_app_paintable(cpu_draw_area, TRUE);

	/* ��ͼ��Ĭ�ϴ�С */
	gtk_drawing_area_size(GTK_DRAWING_AREA(cpu_draw_area), 40, 40);
	/* ������ӵ�cpu���߿���� */
	gtk_container_add(GTK_CONTAINER(CPU_frame1), cpu_draw_area);
	gtk_widget_show(cpu_draw_area);
	/* �����ص����� */
	g_signal_connect(cpu_draw_area, "expose_event",G_CALLBACK(cpu_expose_event), NULL);
	g_signal_connect(cpu_draw_area, "configure_event", G_CALLBACK(cpu_configure_event), NULL);
	gtk_widget_show(cpu_draw_area);


	/* ȡ��CPU��Ϣ */
	char modeName[50], cpuMHz[20], cpuCores[20];
	char cpuBuffer[1000];

	//�õ�CPU������
	getInforNotd("/proc/cpuinfo", "model name", modeName);
	getInforNotd("/proc/cpuinfo", "cpu MHz", cpuMHz);
	getInforNotd("/proc/cpuinfo", "cpu cores", cpuCores);



	/* ��һ��frame�ڱ�һ�°벿��ʾcpu��Ϣ */
	GtkWidget *frame_down = gtk_frame_new("system info ");
	gtk_table_attach_defaults(GTK_TABLE(table1), frame_down, 0, 2, 1, 2);
	gtk_widget_show(frame_down);

	gtk_widget_show(table1);

	/* �ڸÿ�ܽ�һ����񣬽��°벿��Ϊ1�У�2�� */
	GtkWidget *table_down = gtk_table_new(1, 2, TRUE);
	/* ���������ӵ������� */
	gtk_container_add(GTK_CONTAINER(frame_down), table_down);

	/* ��һ��frame�ڱ����벿��ʾӲ����Ϣ */
	GtkWidget *frame_left_down = gtk_frame_new("hardware");
	gtk_table_attach_defaults(GTK_TABLE(table_down), frame_left_down, 0, 1, 0, 1);

	gtk_widget_show(frame_left_down);


	gtk_widget_show(table_down);

	/* ���� */
	vbox = gtk_vbox_new(FALSE, 3);
	gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);
	gtk_container_add(GTK_CONTAINER(frame_left_down), vbox);
	gtk_widget_show(vbox);
	/* ��һ����ǩ��ʾcpu�ͺ� */
	strcpy(cpuBuffer, "CPU model and frequency:\n");
	strcat(cpuBuffer, modeName);

	/* ��һ����ǩ��ʾcpu��Ƶ */
	strcat(cpuBuffer, "\n\ncpu frequency:");
	strcat(cpuBuffer, cpuMHz);
	strcat(cpuBuffer, " MHz");
	/* ��һ����ǩ��ʾcpu���� */
	strcat(cpuBuffer, "\n\ncpu number:");
	strcat(cpuBuffer, cpuCores);

	label = gtk_label_new(cpuBuffer);
	gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 5);
	gtk_widget_show(label);

	//��ʾCPUʹ����
	cpu_label = gtk_label_new("\nCPU using rate:\n");
	gtk_box_pack_start(GTK_BOX(vbox), cpu_label, FALSE, FALSE, 5);
	gtk_widget_show(cpu_label);

	/* ��һ��frame�ڱ���Ұ벿��ʾϵͳ�汾�ź� */
	GtkWidget *frame_right_down = gtk_frame_new("Ubuntu");
	gtk_table_attach_defaults(GTK_TABLE(table_down), frame_right_down, 1, 2, 0, 1);

	gtk_widget_show(frame_right_down);


	gtk_widget_show(table_down);

	/* ���� */
	vbox = gtk_vbox_new(FALSE, 5);
	gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);
	gtk_container_add(GTK_CONTAINER(frame_right_down), vbox);
	gtk_widget_show(vbox);
	/* ��һ����ǩ��ʾcpu�ͺ� */
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
//��ͼ����
gint refresh(gpointer data)
{
	/* cpu */
	/* ��һ�����λ�ͼ�� */
	GtkWidget *drawarea = (GtkWidget *)data;
	GdkDrawable *drawable = drawarea->window;
	GdkGC *gc_chart = gdk_gc_new(drawarea->window);

	/* ������ɫ */
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
	//���cpu������
	char rate[30];

	sprintf(rate, "\nCPU using rate:\n%.1f %\n", cpurate);
	gtk_label_set_text(cpu_label, rate);

	coory[119] = (unsigned)(height - cpurate / 50 * height);
	for (x = 0; x < 119; x++) coory[x] = coory[x + 1];
	int step = width / 120;


	for (x = 119; x >= 0; x--) {
		gdk_draw_line(drawable, gc_chart, x*step, coory[x], (x - 1)*step, coory[x - 1]);
	}
	/* ����һ�� timeout ����,���� TRUE,���������ܹ����������� */
	return TRUE;
}

#endif /* CPU_H_ */
