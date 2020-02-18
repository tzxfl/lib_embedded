ip link set can0 type cantq 125 prop-seg 6phase-seg1 7 phase-seg2 2 sjw 1
Ҳ����ʹ�� ip ����ֱ���趨λ���ʣ�

ip link set can0 type can bitrate 125000
��������ɺ󣬿���ͨ������������ѯ can0 �豸�Ĳ������ã�

ip -details link show can0
��������ɺ󣬿���ʹ�����������ʹ�� can0 �豸��

ifconfig can0 up
ʹ�����������ȡ�� can0 �豸ʹ�ܣ�

ifconfig can0 down
���豸�����У�����ʹ���������������ѯ����״̬��

ip -details -statistics link show can0
Linux ϵͳ��CAN �ӿ�Ӧ�ó��򿪷�

����ϵͳ�� CAN �豸��Ϊ�����豸���й�������� CAN ����Ӧ�ÿ������棬 Linux �ṩ��SocketCAN �ӿڣ�ʹ�� CAN ����ͨ�Ž����ں���̫����ͨ�ţ�Ӧ�ó��򿪷��ӿ� ����ͨ�ã� Ҳ������

���⣬ͨ�� https://gitorious.org/linux-can/can-utils ��վ�����Ļ��� SocketCAN �� can-utils �����׼��� Ҳ����ʵ�ּ��׵� CAN ����ͨ�š�

����������ʹ�� SocketCAN ʵ��ͨ��ʱʹ�õ�Ӧ�ó��򿪷��ӿڡ�

(1). ��ʼ��

SocketCAN �д󲿷ֵ����ݽṹ�ͺ�����ͷ�ļ� linux/can.h �н����˶��塣 CAN �����׽��ֵĴ������ñ�׼�������׽��ֲ�������ɡ������׽�����ͷ�ļ� sys/socket.h �ж��塣 �׽��ֵĳ�ʼ���������£�


1	int s;
2	struct sockaddr_can addr;
3	struct ifreq ifr;
4	s = socket(PF_CAN, SOCK_RAW, CAN_RAW);//���� SocketCAN �׽���
5	strcpy(ifr.ifr_name, "can0" );
6	ioctl(s, SIOCGIFINDEX, &ifr);//ָ�� can0 �豸
7	addr.can_family = AF_CAN;
8	addr.can_ifindex = ifr.ifr_ifindex;
9	bind(s, (struct sockaddr *)&addr, sizeof(addr)); //���׽����� can0 ��
(2). ���ݷ���

�������շ������ݷ��棬 CAN �������׼�׽���ͨ�����в�ͬ��ÿһ��ͨ�Ŷ����� can_ frame �ṹ�彫���ݷ�װ��֡�� �ṹ�嶨�����£�

1	struct can_frame {
2	canid_t can_id;//CAN ��ʶ��
3	__u8 can_dlc;//���ݳ��ĳ���
4	__u8 data[8];//����
5	};
can_id Ϊ֡�ı�ʶ���� ����������Ǳ�׼֡�� ��ʹ�� can_id �ĵ� 11 λ�� ���Ϊ��չ֡�� ��ʹ�� 0�� 28 λ�� can_id �ĵ� 29�� 30�� 31 λ��֡�ı�־λ����������֡�����ͣ��������£�

1	#define CAN_EFF_FLAG 0x80000000U //��չ֡�ı�ʶ
2	#define CAN_RTR_FLAG 0x40000000U //Զ��֡�ı�ʶ
3	#define CAN_ERR_FLAG 0x20000000U //����֡�ı�ʶ�����ڴ�����
���ݷ���ʹ�� write ������ʵ�֡� ������͵�����֡(��ʶ��Ϊ 0x123)���������ֽ�(0xAB)�����ݣ��ɲ������·������з��ͣ�

1	struct can_frame frame;
2	frame.can_id = 0x123;//���Ϊ��չ֡����ô frame.can_id = CAN_EFF_FLAG | 0x123;
3	frame.can_dlc = 1; //���ݳ���Ϊ 1
4	frame.data[0] = 0xAB; //��������Ϊ 0xAB
5	int nbytes = write(s, &frame, sizeof(frame)); //��������
6	if(nbytes != sizeof(frame)) //��� nbytes ������֡���ȣ���˵������ʧ��
7	printf("Error\n!");
���Ҫ����Զ��֡(��ʶ��Ϊ 0x123)���ɲ������·������з��ͣ�

1	struct can_frame frame;
2	frame.can_id = CAN_RTR_FLAG | 0x123;
3	write(s, &frame, sizeof(frame));
(3). ���ݽ���

���ݽ���ʹ�� read ��������ɣ�ʵ�����£�

1	struct can_frame frame;
2	int nbytes = read(s, &frame, sizeof(frame));
��Ȼ�� �׽��������շ�ʱ���õ� send�� sendto�� sendmsg �Լ���Ӧ�� recv ����Ҳ���������� CAN�������ݵ��շ���

(4). ������

��֡���պ󣬿���ͨ���ж� can_id �е� CAN_ERR_FLAG λ���жϽ��յ�֡�Ƿ�Ϊ����֡�� ���Ϊ����֡������ͨ�� can_id ����������λ���жϴ���ľ���ԭ��

����֡�ķ���λ��ͷ�ļ� linux/can/error.h �ж��塣

(5). ���˹�������

�����ݽ���ʱ��ϵͳ���Ը���Ԥ�����õĹ��˹���ʵ�ֶԱ��ĵĹ��ˡ����˹���ʹ�� can_filter �ṹ����ʵ�֣��������£�

1	struct can_filter {
2	canid_t can_id;
3	canid_t can_mask;
4	};
���˵Ĺ���Ϊ��

���յ�������֡�� can_id  & mask == can_id & mask

ͨ���������������ϵͳ�й��˵����в����Ϲ���ı��ģ�ʹ��Ӧ�ó�����Ҫ���޹صı��Ľ��д����� can_filter �ṹ�� can_id �У�����λ CAN_INV_FILTER ����λʱ����ʵ�� can_id ��ִ�й���ǰ��λ��ת��

�û�����Ϊÿ���򿪵��׽������ö��������Ĺ��˹���ʹ�÷������£�

1	struct can_filter rfilter[2];
2	rfilter[0].can_id = 0x123;
3	rfilter[0].can_mask = CAN_SFF_MASK; //#define CAN_SFF_MASK 0x000007FFU
4	rfilter[1].can_id = 0x200;
5	rfilter[1].can_mask = 0x700;
6	setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter));//���ù���
�ڼ�������£����Ӧ�ó�����Ҫ���ձ��ģ����Խ��ù��˹��������Ļ���ԭʼ�׽��־ͻ�������н��յ��ı��ġ������ֽ����������ݵ�Ӧ���У��������ں���ʡ�Խ��ն��У��Դ˼��� CPU ��Դ�����ġ����÷������£�

1	setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0); //���ù��˹���
ͨ�������������ʵ�ֶԴ���֡�Ĺ��ˣ� ���磺

1	can_err_mask_t err_mask = ( CAN_ERR_TX_TIMEOUT | CAN_ERR_BUSOFF );
2	setsockopt(s, SOL_CAN_RAW, CAN_RAW_ERR_FILTER, err_mask, sizeof(err_mask));
(6). �ػ���������

��Ĭ������£� ���ػػ������ǿ����ģ�����ʹ������ķ����رջػ�/�������ܣ�

1	int loopback = 0; // 0 ��ʾ�ر�, 1 ��ʾ����( Ĭ��)
2	setsockopt(s, SOL_CAN_RAW, CAN_RAW_LOOPBACK, &loopback, sizeof(loopback));
�ڱ��ػػ����ܿ���������£����еķ���֡���ᱻ�ػ����� CAN ���߽ӿڶ�Ӧ���׽����ϡ� Ĭ������£����� CAN ���ĵ��׽��ֲ�������Լ����͵ı��ģ���˷����׽����ϵĻػ������ǹرյġ���������Ҫ��ʱ��ı���һĬ����Ϊ��

1	int ro = 1; // 0 ��ʾ�ر�( Ĭ��), 1 ��ʾ����
2	setsockopt(s, SOL_CAN_RAW, CAN_RAW_RECV_OWN_MSGS, &ro, sizeof(ro));
