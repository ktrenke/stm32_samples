

//PA9  TX
//PA10 RX


void PUT32 ( unsigned int, unsigned int );
unsigned int GET32 ( unsigned int );
void dummy ( unsigned int );
void ASMDELAY ( unsigned int );


#define GPIOABASE 0x40010800
#define GPIOA_BASE 0x40010800
#define GPIOA_CRL  (GPIOA_BASE+0x00)
#define GPIOA_CRH  (GPIOA_BASE+0x04)
#define GPIOA_IDR  (GPIOA_BASE+0x08)
#define GPIOA_ODR  (GPIOA_BASE+0x0C)
#define GPIOA_BSRR (GPIOA_BASE+0x10)
#define GPIOA_BRR  (GPIOA_BASE+0x14)
#define GPIOA_LCKR (GPIOA_BASE+0x18)

#define RCC_BASE 0x40021000
#define RCC_CR      (RCC_BASE+0x00)
#define RCC_CFGR    (RCC_BASE+0x04)
#define RCC_APB2ENR (RCC_BASE+0x18)
#define RCC_APB1ENR (RCC_BASE+0x1C)
#define RCC_BDCR    (RCC_BASE+0x20)

#define PWR_BASE 0x40007000
#define PWR_CR  (PWR_BASE + 0x00)
#define PWR_CSR (PWR_BASE + 0x04)

#define RTC_BASE 0x40002800
#define RTC_CRL (RTC_BASE + 0x04)
#define RTC_PRLL (RTC_BASE + 0x0C)
#define RTC_CNTL (RTC_BASE + 0x1C)

static void clock_init ( void )
{
    unsigned int ra;

    //enable the external clock
    ra=GET32(RCC_CR);
    ra=ra|1<<16; //HSEON
    PUT32(RCC_CR,ra);

    //wait for HSE to settle
    while(1) if(GET32(RCC_CR)&(1<<17)) break; //HSERDY

    //select HSE clock
    ra=GET32(RCC_CFGR);
    ra&=~(0x3<<0);
    ra|= (0x1<<0);
    PUT32(RCC_CFGR,ra);
    //wait for it
    while(1) if((GET32(RCC_CFGR)&0xF)==0x5) break;
}


static void rtoff ( void )
{
    unsigned int ra;
    while(1)
    {
        ra=GET32(RTC_CRL);
        if(ra&(1<<5)) break;
    }
}

static void rtc_init ( void )
{
    unsigned int ra;
    unsigned int bdcr;

    ra=GET32(RCC_APB1ENR);
    ra|=1<<28; //PWR
    PUT32(RCC_APB1ENR,ra);

    ra=GET32(PWR_CR);
    ra|=1<<8; //DBP
    PUT32(PWR_CR,ra);


    PUT32(RCC_BDCR,0x10000);
    PUT32(RCC_BDCR,0x00000);

    bdcr=GET32(RCC_BDCR);
//hexstring(bdcr);
    bdcr|=1<<0; //LSEON
    PUT32(RCC_BDCR,bdcr);

    while(1)
    {
        bdcr=GET32(RCC_BDCR);
        if(bdcr&(1<<1)) break; //LSERDY
    }

    //bdcr=GET32(RCC_BDCR);
    bdcr&=~(3<<8); //RTCSEL should be zero
    bdcr|= (1<<8); //RTCSEL LSE
    PUT32(RCC_BDCR,bdcr);

    //bdcr=GET32(RCC_BDCR);
    bdcr|= (1<<15); //RTCEN
    PUT32(RCC_BDCR,bdcr);
//hexstring(bdcr);

    while(1)
    {
        ra=GET32(RTC_CRL);
        if(ra&(1<<3)) break; //RSF
    }
    rtoff();
    PUT32(RTC_CRL,0x38);
    PUT32(RTC_PRLL,32767);
    PUT32(RTC_CRL,0x28);
    rtoff();
}


static unsigned char last_time[4];

static unsigned int ht,ho;
static unsigned int mt,mo;
static unsigned int st,so;



static void ht_delay ( void )
{
    ASMDELAY(1);
}

#define DATA    0
#define WR      1
#define CS      2

//  777
// 3   6
// 3   6
//  222
// 1   5
// 1   5
// 4000
static const unsigned char num_bits[16]=
{
0xEB,//0 765x3x10
0x60,//1 x65xxxxx
0xC7,//2 76xxx210
0xE5,//3 765xx2x0
0x6C,//4 x65x32xx
0xAD,//5 7x5x32x0
0xAF,//6 7x5x3210
0xE0,//7 765xxxxx
0xEF,//8 765x3210
0xED,//9 765x32x0
0x00,
0x00,
0x00,
0x00,
0x00,
0x00
};

static void ht_pin_init ( void )
{
    unsigned int ra;

    ra=GET32(RCC_APB2ENR);
    ra|=1<<2; //enable port a
    PUT32(RCC_APB2ENR,ra);
    //config
    ra=GET32(GPIOABASE+0x00);
    ra&=~(3<<((DATA<<2)+0));   //PA0
    ra|= (3<<((DATA<<2)+0));   //PA0
    ra&=~(3<<((DATA<<2)+2));   //PA0
    ra|= (0<<((DATA<<2)+2));   //PA0
    ra&=~(3<<((WR<<2)+0));   //PA1
    ra|= (3<<((WR<<2)+0));   //PA1
    ra&=~(3<<((WR<<2)+2));   //PA1
    ra|= (0<<((WR<<2)+2));   //PA1
    ra&=~(3<<((CS<<2)+0));   //PA2
    ra|= (3<<((CS<<2)+0));   //PA2
    ra&=~(3<<((CS<<2)+2));   //PA2
    ra|= (0<<((CS<<2)+2));   //PA2
    PUT32(GPIOABASE+0x00,ra);
    PUT32(GPIOABASE+0x10,(1<<DATA)|(1<<WR)|(1<<CS));
}
static void ht_data ( unsigned int x )
{
    if(x) PUT32(GPIOABASE+0x10,(1<<(DATA+ 0)));
    else  PUT32(GPIOABASE+0x10,(1<<(DATA+16)));
}
static void ht_wr ( unsigned int x )
{
    if(x) PUT32(GPIOABASE+0x10,(1<<(WR+ 0)));
    else  PUT32(GPIOABASE+0x10,(1<<(WR+16)));
}
static void ht_cs ( unsigned int x )
{
    if(x) PUT32(GPIOABASE+0x10,(1<<(CS+ 0)));
    else  PUT32(GPIOABASE+0x10,(1<<(CS+16)));
}
static void ht_strobe_wr ( void )
{
    ht_delay();
    ht_wr(0);
    ht_delay();
    ht_wr(1);
    ht_delay();
}
static void ht_command ( unsigned int x )
{
    unsigned int ra;
    x&=0xFF;
    x|=0x400;
    x<<=1;

    ht_delay();
    ht_cs(0);
    for(ra=0x800;ra;ra>>=1)
    {
        ht_data(ra&x);
        ht_strobe_wr();
    }
    ht_cs(1);
    ht_delay();
}
static void ht_write ( unsigned int a, unsigned int d )
{
    unsigned int ra;
    unsigned int x;
    x=0x5; //3
    x<<=6;
    x|=a&0x3F; //6
    x<<=8;
    x|=d&0xFF; //8
    x<<=1; //1
    ht_delay();
    ht_cs(0);
    //3+6+8+1 = 18
    for(ra=1<<(18-1);ra;ra>>=1)
    {
        ht_data(ra&x);
        ht_strobe_wr();
    }
    ht_cs(1);
    ht_delay();
}
static void ht_init ( void )
{
    unsigned int ra;
    unsigned int add;
    ht_pin_init();
    ht_command(0x00); //SYS DIS
    ASMDELAY(100000);
    ht_command(0x01); //SYS EN
    ht_command(0x18); //RC 256K default setting
    ht_command(0x29); //4 commons 1/3 bias
    ht_command(0x03); //LCD ON
    add=0;
    for(ra=0;ra<16;ra++)
    {
        ht_write(add,0x00);
        add+=2;
    }
}

void show_clock ( void )
{
    if(last_time[0]==ht)
    if(last_time[1]==ho)
    if(last_time[2]==mt)
    if(last_time[3]==mo)
    return;

    last_time[0]=ht&0xF;
    last_time[1]=ho&0xF;
    last_time[2]=mt&0xF;
    last_time[3]=mo&0xF;

    
    ht_write(0,num_bits[last_time[3]]);
    ht_write(2,num_bits[last_time[2]]);
    ht_write(4,num_bits[last_time[1]]);
    if(last_time[0]==0)
    {
        ht_write(6,0x00);
    }
    else
    {
        ht_write(6,num_bits[last_time[0]]);
    }
}


int notmain ( void )
{
    unsigned int ra;
    unsigned int rb;
    unsigned int rc;
    unsigned int rd;
    
    clock_init(); //still 8mhz just more accurate.

    rtc_init();
    ht_init();

    last_time[0]=0;
    last_time[1]=0;
    last_time[2]=0;
    last_time[3]=0;

    ht=1;ho=1;
    mt=0;mo=7;
    st=0;so=0;

    show_clock();

    rb=GET32(RTC_CNTL);
    while(1)
    {
        ra=GET32(RTC_CNTL);
        if(ra!=rb)
        {
            rc=ra-rb;
            for(rd=0;rd<rc;rd++)
            {
                so++;
                if(so>9)
                {
                    so=0;
                    st++;
                    if(st>5)
                    {
                        st=0;
                        mo++;
                    }
                }
                if(mo>9)
                {
                    mo=0;
                    mt++;
                    if(mt>5)
                    {
                        mt=0;
                        ho++;
                    }
                }
                if(ht)
                {
                    if(ho>2)
                    {
                        ht=0;
                        ho=1;
                    }
                }
                else
                {
                    if(ho>9)
                    {
                        ho=0;
                        ht++;
                    }
                }
            }
            rb=ra;
            show_clock();
        }
    }

    return(0);
}
