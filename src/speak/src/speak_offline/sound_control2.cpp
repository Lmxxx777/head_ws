#include <vlc/vlc.h>
#include <unistd.h>

int main(void){
    //VLC 播放器实例(可以理解成VLC引擎)
    libvlc_instance_t * inst;
    //VLC 媒体播放器(播放视频时控制, 播放、暂停、停止、音量...)
    libvlc_media_player_t * vlc_player = NULL;
    //播放视频来源,和视频文件的信息(本地视频文件,网络视频文件,RTSP...)
    libvlc_media_t * vlc_media = NULL;
    //初始化VLC引擎
    inst = libvlc_new (0, NULL);
    //指定本地根目录下/test.mp4为视频来源,请按实际路径指定,可以去https://www.haolizi.net/example/view_20786.html下载各种格式测试视频
    vlc_media = libvlc_media_new_path(inst, "/home/lmx/head_ws/src/speak/resource/speak.wav/home/lmx/head_ws/src/speak/resource/speak.wav");
    //创建VLC视频播放器,根据vlc_media
    vlc_player = libvlc_media_player_new_from_media(vlc_media);
    //以下代码暂时不使用vlc_media指针了，所以提前释放了
    libvlc_media_release (vlc_media);
    //设置播放容器,0x4800006并不是死值,这是X11的窗口ID,下边简单说一下获取窗口ID方法
    //也可以不调用此方法,或者窗口ID参数处也可以随意填写一个int值, VLC已做判断,没有找不到指定窗口ID时或没设置窗口ID时都会创建一个窗口播放视频
    //首次调用建议先不要调用此api
    libvlc_media_player_set_xwindow(vlc_player, 0x4800006);
    //播放视频,返回0 表示已开始播放,非0表示播放失败
    int result = libvlc_media_player_play(vlc_player);
    //睡1秒,如果不睡,在后续调用libvlc_media_player_is_playing时会返回0(0:当前未播放, 1:正在播放)！！！,应该是播放视频也需要一些时间,1秒足够
    //不一定必须睡一秒,对libvlc api还不是特别的解,如果您对libvlc非常了解,不要参考此处...
    sleep(1);
    //设置音量,可以发现在控制视频时都需要用到vlc_player指针
    libvlc_audio_set_volume(vlc_player, 50);
    //循环判断当前视频是否是播放状态
    while(libvlc_media_player_is_playing(vlc_player)){
        sleep(1);
    }
    //播放完毕,释放播放器
    libvlc_media_player_release (vlc_player);
    //释放引擎
    libvlc_release (inst);
    return 0;
}