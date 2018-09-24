var result = [];
var vbat = 0.0;
var vfan = 0.0;
var vsolar = 0.0;
var cc = 0.0;
let token = "c01678a1ded946cdb3f12990c534d23f";

initWelcome();
initMorris();
setInterval(function () {
    let currentdate = new Date();
    let datetime = currentdate.getFullYear() + "-"
        + (currentdate.getMonth() + 1) + "-"
        + currentdate.getDate() + " "
        + currentdate.getHours() + ":"
        + currentdate.getMinutes() + ":"
        + currentdate.getSeconds();

    $.get("http://blynk-cloud.com/" + token + "/get/V0", function (data) {
        vbat = data[0];
    });
    $.get("http://blynk-cloud.com/" + token + "/get/V1", function (data) {
        vfan = data[0]
    });
    $.get("http://blynk-cloud.com/" + token + "/get/V2", function (data) {
        vsolar = data[0]
    });
    $.get("http://blynk-cloud.com/" + token + "/get/V3", function (data) {
        cc = data[0]
    });

    $.get("http://blynk-cloud.com/" + token + "/isHardwareConnected", function (data) {
        if (data) {
            $('#statusDevice').html("ONLINE");
            $('#statusD').attr('class', 'connected');
            $('#vBatt').html(vbat + " V");
            $('#vFan').html(vfan + " V");
            $('#vSolar').html(vsolar + " V");
            $('#aChangr').html(cc + " A");
            result.push({"period": datetime, "vbat": vbat, "vfan": vfan, "vsolar": vsolar, "ccharging": 12.7});
            setMorris(result); // Array now contains 1 item
        } else {
            $('#statusDevice').html("OFFLINE");
            $('#statusD').attr('class', 'disconnect');
            $('#vBatt').html("offline");
            $('#vFan').html("offline");
            $('#vSolar').html("offline");
            $('#aChangr').html("offline");
        }
    });

    if (result.length > 5) {
        result.splice(0, 1);
    }
    console.log(result.length);
}, 2000);

function initWelcome() {
    $.gritter.add({
        title: 'Welcome to Dashio!',
        text: 'Hover me to enable the Close Button. You can hide the left sidebar clicking on the button next to the logo.',
        image: 'img/ui-sam.jpg',
        sticky: false,
        time: 8000,
        class_name: 'my-sticky-class'
    });
}

function initMorris() {
    morrisLine = Morris.Area({
        element: 'hero-area',
        xkey: 'period',
        ykeys: ['vbat', 'vfan', 'vsolar', 'ccharging'],
        labels: ['VBattery', 'VFan', 'VSolar', 'Current Charging'],
        lineWidth: 1,
        pointSize: 10,
        lineColors: ['#4a8bc2', '#ff6c60', '#a9d86e', '#a0d86e'],
        fillOpacity: 0.5,
        smooth: true,
        xLabels: "5sec",
        resize: true,
        ymin :'0.5',
        ymax: 'auto'
    });
}

function setMorris(data1) {
    morrisLine.setData(data1);
}