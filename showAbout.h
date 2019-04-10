#ifndef SHOWABOUT_H_
#define SHOWABOUT_H_


GtkWidget *t_label; //用来显示当前时间
//得到主机名
void get_hostname(char *hostname)
{
	FILE *fp;
	char buf[20];
	fp = fopen("/proc/sys/kernel/hostname", "r");
	if (fp == NULL) {
		printf("open failed!\n");
		return;
	}
	int ret = fread(hostname, sizeof(char), 20, fp);
	hostname[ret - 1] = '\0';
	fclose(fp);
}
//得到运行时间
void get_runtime(float *runtime) {
	FILE *fp;
	char time[50];
	fp = fopen("/proc/uptime", "r");
	fgets(time, sizeof(time), fp);
	sscanf(time, "%f", runtime);
	fclose(fp);
}
//得到启动时间
void get_starttime(char *starttime)
{
	float runtime;
	time_t ttime;
	struct tm *t;
	ttime = time(NULL);  //从1970 01 01起
	get_runtime(&runtime);
	ttime = ttime - runtime;
	t = localtime(&ttime);
	strcpy(starttime, asctime(t));
}
//得到系统类型
void get_ostype(char *ostype)
{
	FILE *fp, *fd;
	char type[20];
	char release[20];
	char buf[20];
	fp = fopen("/proc/sys/kernel/ostype", "r");
	fd = fopen("/proc/sys/kernel/osrelease", "r");
	if (fp == NULL || fd == NULL) {
		printf("open error!\n");
		return;
	}
	fgets(type, sizeof(type), fp);
	fgets(release, sizeof(release), fd);
	sscanf(type, "%s", ostype);
	sscanf(release, "%s", buf);
	strcat(ostype, " ");
	strcat(ostype, buf);
	fclose(fp);
	fclose(fd);
	return;
}
//关机绑定事件
void power_off(void)
{
	system("sudo halt");
}
void createAboutPage(GtkWidget* notebook)
{
	GtkWidget *frame = gtk_frame_new("message about");
	gtk_container_set_border_width(GTK_CONTAINER(frame), 10);
	gtk_widget_set_size_request(frame, 120, 500);
	gtk_widget_show(frame);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), frame, gtk_label_new("sys message"));
	//创建一个4*4的表格
	GtkWidget *table = gtk_table_new(4, 4, TRUE);
	gtk_container_add(GTK_CONTAINER(frame), table);

	GtkWidget *s_frame = gtk_frame_new("system information");
	gtk_table_attach_defaults(GTK_TABLE(table), s_frame, 0, 4, 0, 3);
	gtk_widget_show(s_frame);

	char hostname[50], starttime[50], runtime[50], ostype[50];
	float run_time;
	get_hostname(hostname);
	get_runtime(&run_time);
	int seconds = (long)run_time % 60;  //秒
	int min = (long)(run_time / 60) % 60;//分钟
	int hour = (long)(run_time / 3600) % 60;//小时
	sprintf(runtime, "%ld:%ld:%ld", hour, min, seconds);
	get_starttime(starttime);
	get_ostype(ostype);

	char buffer[200]="\0";
	strcat(buffer, "\nhostname: ");
	strcat(buffer, hostname);
	strcat(buffer, "\n\nstart time: ");
	strcat(buffer, starttime);
	strcat(buffer, "\nrun time: ");
	strcat(buffer, runtime);
	strcat(buffer, "\n\nostype: ");
	strcat(buffer, ostype);

	GtkWidget *vbox1 = gtk_vbox_new(FALSE, 2);
	gtk_container_set_border_width(GTK_CONTAINER(vbox1), 10);
	gtk_container_add(GTK_CONTAINER(s_frame), vbox1);
	gtk_widget_show(vbox1);
	gtk_widget_show(table);

	char author[50] = "\0";
	strcat(author, "\n\nauthor: Wangsen");
	strcat(author, "\n\nEmail: 771064137@163.com");
	GtkWidget *label1 = gtk_label_new(author);
	gtk_box_pack_start(GTK_BOX(vbox1), label1, FALSE, FALSE, 1);
	gtk_widget_show(label1);

	GtkWidget *label2 = gtk_label_new(buffer);
	gtk_box_pack_start(GTK_BOX(vbox1), label2, FALSE, FALSE, 1);
	gtk_widget_show(label2);
	//创建新框架显示现在时间
	GtkWidget *t_frame = gtk_frame_new("now time");
	gtk_table_attach_defaults(GTK_TABLE(table), t_frame, 0, 3, 3, 4);
	gtk_widget_show(t_frame);

	//设置盛放时间的组件
	GtkWidget *vbox2 = gtk_vbox_new(FALSE, 1);
	gtk_container_set_border_width(GTK_CONTAINER(vbox2), 10);
	gtk_container_add(GTK_CONTAINER(t_frame), vbox2);
	gtk_widget_show(vbox2);
	gtk_widget_show(table);

	t_label = gtk_label_new("now time");
	gtk_box_pack_start(GTK_BOX(vbox2), t_label, FALSE, FALSE, 1);
	gtk_widget_show(t_label);

	//增加关机按钮
	GtkWidget *button_frame = gtk_frame_new("Power off");
	gtk_table_attach_defaults(GTK_TABLE(table), button_frame, 3, 4, 3, 4);
	gtk_widget_show(button_frame);

	GtkWidget *power_label = gtk_button_new_with_label("power off");
	gtk_container_add(GTK_CONTAINER(button_frame), power_label);
	g_signal_connect(G_OBJECT(power_label), "clicked", G_CALLBACK(power_off), NULL);
	GtkWidget *tooltip = gtk_tooltips_new();  //设置提示信息
	gtk_tooltips_set_tip(tooltip, power_label, "click to power off", NULL);
	gtk_widget_show(power_label);
	
}
//得到当前时间
void get_nowtime(char *nowtime)
{
	time_t ttime;
	struct tm *t;
	ttime = time(NULL);
	t = localtime(&ttime);
	sprintf(nowtime, "%s", asctime(t));
}
gint time_refresh(void)
{
	char now_time[30] = "\0";
	char buff[50] = "now time:";
	get_nowtime(now_time);
	strcat(buff, now_time);
	gtk_label_set_text(t_label, buff);
	return TRUE;
}
#endif /* SHOWABOUT_H_ */
