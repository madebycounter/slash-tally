import "./style.css";

window.onload = () => {
    var input_wifi_ssid = document.getElementById(
        "input_wifi_ssid"
    ) as HTMLInputElement;
    var input_wifi_password = document.getElementById(
        "input_wifi_password"
    ) as HTMLInputElement;
    var input_camera = document.getElementById(
        "input_camera"
    ) as HTMLInputElement;
    var input_brightness = document.getElementById(
        "input_brightness"
    ) as HTMLInputElement;
    var label_brightness = document.getElementById(
        "label_brightness"
    ) as HTMLParagraphElement;
    var button_save = document.getElementById(
        "button_save"
    ) as HTMLButtonElement;
    var button_restart = document.getElementById(
        "button_restart"
    ) as HTMLButtonElement;
    var button_signal = document.getElementById(
        "button_signal"
    ) as HTMLButtonElement;

    fetch("/api/config_state")
        .then((resp) => resp.json())
        .then((dat) => {
            input_wifi_ssid.value = dat.wifi_ssid;
            input_wifi_password.value = dat.wifi_password;
            input_camera.value = dat.camera;
            input_brightness.value = dat.brightness;
            label_brightness.innerHTML = dat.brightness;
        });

    input_brightness.onchange = () => {
        label_brightness.innerHTML = input_brightness.value;
    };

    input_brightness.oninput = () => {
        label_brightness.innerHTML = input_brightness.value;
    };

    button_save.onclick = () => {
        var wifi_ssid = input_wifi_ssid.value;
        var wifi_password = input_wifi_password.value;
        var camera = input_camera.value;
        var brightness = input_brightness.value;

        fetch(
            `/api/config?wifi_ssid=${wifi_ssid}&wifi_password=${wifi_password}&camera=${camera}&brightness=${brightness}`
        ).then(() => {
            alert("Done");
        });
    };

    button_restart.onclick = () => {
        fetch("/api/restart").then(() => {
            alert("Done");
        });
    };

    button_signal.onclick = () => {
        fetch("/api/signal?type=solid").then(() => {
            alert("Done");
        });
    };
};
