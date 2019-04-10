#ifndef PROCESS_H_
#define PROCESS_H_

GtkWidget *clist;
GtkWidget *label_proc;
GtkWidget *label_time;
gint selectedRow;
/* ˢ����Ϣ */
void set_proc_info();
/* ��ʾ��ȡ��i���ո�֮ǰ��һ���ַ���(�˼���õ�����)) */
void read_info(char store[], int i, char get[]);
/* �û�ѡ��ĳһ��ʱ�Ļص�����*/
void selection_made(GtkWidget *clist, gint row, gint column,GdkEventButton *event, gpointer data);
struct PID_INFO {    //������Ϣ
	char name[30];
	char pid[10];
	char ppid[10];
	char rss[10];
	char priority[10];
}pidinfo[40960];
//��������
void creat_process(void)
{
	system("./main");
}
void createProPage(GtkWidget* notebook)
{
	char buffer[100];
	GtkWidget * table;
	GtkWidget *frame_down;
	GtkWidget *tooltip;


	GtkWidget *frame = gtk_frame_new("process infomation");
	gtk_container_set_border_width(GTK_CONTAINER(frame), 10);
	gtk_widget_set_size_request(frame, 100, 355);
	gtk_widget_show(frame);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), frame, gtk_label_new("pro_info"));



	/* ��һ�����10��3�У� */
	table = gtk_table_new(10, 3, TRUE);
	/* ���������ӵ���� */
	gtk_container_add(GTK_CONTAINER(frame), table);

	//��ʾ���н�����Ϣ
	frame_down = gtk_frame_new("process info table");
	gtk_table_attach_defaults(GTK_TABLE(table), frame_down, 0, 3, 0, 9);
	gtk_widget_show(frame_down);
	gtk_widget_show(table);

	/* ������*/
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
	gtk_container_add(GTK_CONTAINER(frame_down), scrolled_window);
	gtk_widget_show(scrolled_window);

	gchar *titles[5] =
	{ "pro_name", "pro_PID", "pro_stat", "occupied memory", "priority" };

	/* ����GtkCList�����������У�����ʹ����5��*/
	clist = gtk_clist_new_with_titles(5, titles);

	tooltip = gtk_tooltips_new();
	gtk_tooltips_set_tip(tooltip, clist, "click to kill process", NULL);  //��ʾ
	gtk_signal_connect(GTK_OBJECT(clist), "select_row", GTK_SIGNAL_FUNC(selection_made), NULL);   //���¼�
	/* ����ÿһ�е���� */
	gtk_clist_set_column_width(GTK_CLIST(clist), 0, 200);
	gtk_clist_set_column_width(GTK_CLIST(clist), 1, 120);
	gtk_clist_set_column_width(GTK_CLIST(clist), 2, 110);
	gtk_clist_set_column_width(GTK_CLIST(clist), 3, 100);
	gtk_clist_set_column_width(GTK_CLIST(clist), 4, 100);
	gtk_container_add(GTK_CONTAINER(scrolled_window), clist);
	gtk_widget_show(clist);
	set_proc_info();

	//���Ӵ������̰�ť
	GtkWidget *frame_button = gtk_frame_new("creat process");
	gtk_table_attach_defaults(GTK_TABLE(table), frame_button, 1, 2, 9, 10);
	gtk_widget_show(frame_button);

	GtkWidget *process_label = gtk_button_new_with_label("creat process");
	gtk_container_add(GTK_CONTAINER(frame_button), process_label);
	g_signal_connect(G_OBJECT(process_label), "clicked", G_CALLBACK(creat_process), NULL);
	GtkWidget *tooltip1 = gtk_tooltips_new();  //������ʾ��Ϣ
	gtk_tooltips_set_tip(tooltip1, process_label, "click to creat process", NULL);
	gtk_widget_show(process_label);

}
void read_pid(struct PID_INFO *pid, const char *name)
{
	char filename[20];
	char info[200];
	char buf[20];
	FILE *fd;
	sprintf(filename, "/proc/%s/stat", name);
	fd = fopen(filename, "r");
	if (fd == NULL) {
		printf("open error!\n");
		return;
	}
	fgets(info, sizeof(info), fd);
	int len = strlen(info);
	int i = 0, j = 0, k = 0;
	while (len--) {
		if (info[i] == ' ') {
			buf[k] = '\0';
			k = 0;
			j++;
			i++;
			if (j == 1) strcpy(pid->pid, buf);
			if (j == 2) strcpy(pid->name, buf);
			if (j == 4) strcpy(pid->ppid, buf);
			if (j == 18) strcpy(pid->priority, buf);
			if (j == 24) strcpy(pid->rss, buf);
			continue;
		}
		buf[k++] = info[i++];
	}
	int len2 = strlen(pid->name);
	strncpy(buf, pid->name + 1, len2 - 2);
	buf[len2 - 2] = '\0';
	strcpy(pid->name, buf);
	int rss = atoi(pid->rss);
	rss = rss * 4;
	sprintf(pid->rss, "%d", rss);
	fclose(fd);
}
void set_proc_info()
{
	/* ��ȡ��ǰ������Ϣ������ʾ���б���� */
	DIR *dir;
	struct dirent *ptr;
	int i = 0;
	if (!(dir = opendir("/proc"))) return;
	while ((ptr = readdir(dir))) {
		if (ptr->d_name[0] >= '1' && ptr->d_name[0] <= '9')
		{
			read_pid(&(pidinfo[i]), ptr->d_name);
			gchar *list[1][5] = { pidinfo[i].name,pidinfo[i].pid,pidinfo[i].ppid,pidinfo[i].rss,pidinfo[i].priority };
			gtk_clist_append((GtkCList*)clist, list[0]);
			gtk_clist_thaw((GtkCList *)clist);
			i++;
		}
	}
	closedir(dir);
}
//����һ��pidɱ������
void killProcess(gchar *Pid)
{
	gchar killCmd[50] = "sudo kill ";
	strcat(killCmd, Pid);
	system(killCmd);
	gtk_clist_remove(clist, selectedRow);
	/* �½�һ����Ϣ�Ի��� */
	GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_WARNING, GTK_BUTTONS_OK, ("the process is killed")); //��ʾ��Ϣ������
	gtk_window_set_title(GTK_WINDOW(dialog), ("pro_kill"));//�Ի���ı���
	gtk_dialog_run(GTK_DIALOG(dialog));//���жԻ���
	gtk_widget_destroy(dialog);//ɾ���Ի���
}

/* �û�ѡ��ĳһ��ʱ�Ļص�����*/
void selection_made(GtkWidget *clist, gint row, gint column,GdkEventButton *event, gpointer data)
{
	/* ȡ�ô洢�ڱ�ѡ�е��к��еĵ�Ԫ���ϵ��ı�
	 * �������ʱ��������t e x t��������һ��ָ��*/
	gchar *Pid, *procName, buffer[50] = "Kill the process:";
	/* �ú�����ȡCLIST��ѡ�е�PID��
	 * ����gtk_clist_set_text���������෴��*/
	gtk_clist_get_text(GTK_CLIST(clist), row, 1, &Pid);
	gtk_clist_get_text(GTK_CLIST(clist), row, 0, &procName);
	selectedRow = row;
	strcat(buffer, procName);
	strcat(buffer, "?");
	GtkWidget *dialog;
	//���游���ڹر�
	dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO,buffer);
	gtk_window_set_title(GTK_WINDOW(dialog), "kill process");
	GtkResponseType result = gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
	if (result == GTK_RESPONSE_YES || result == GTK_RESPONSE_APPLY)
	{
		killProcess(Pid);
	}
	return;
}


#endif /* PROCESS_H_ */
