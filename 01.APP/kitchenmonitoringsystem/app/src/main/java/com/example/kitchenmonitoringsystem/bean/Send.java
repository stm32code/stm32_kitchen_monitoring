package com.example.kitchenmonitoringsystem.bean;

public class Send {
    private Integer cmd;
    private Integer tmep_v;
    private Integer mq2_v;
    private Integer mq135_v;
    private Integer fan;

    public void setMq135_v(Integer mq135_v) {
        this.mq135_v = mq135_v;
    }

    public void setFan(Integer fan) {
        this.fan = fan;
    }

    public void setRelay(Integer relay) {
        this.relay = relay;
    }

    private Integer relay;
    @Override
    public String toString() {
        return "Send{" +
                "cmd=" + cmd +
                ", tmep_v=" + tmep_v +
                ", mq2_v=" + mq2_v +
                ", mq135_v=" + mq135_v +
                ", fan=" + fan +
                ", relay=" + relay +
                '}';
    }

    public Integer getCmd() {
        return cmd;
    }

    public void setCmd(Integer cmd) {
        this.cmd = cmd;
    }

    public Integer getTmep_v() {
        return tmep_v;
    }

    public void setTmep_v(Integer tmep_v) {
        this.tmep_v = tmep_v;
    }

    public Integer getMq2_v() {
        return mq2_v;
    }

    public void setMq2_v(Integer mq2_v) {
        this.mq2_v = mq2_v;
    }
}
