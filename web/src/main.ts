import "./style.css";

window.onload = () => {
    var current_id: string | null = null;

    var input_tally_id = document.getElementById(
        "input_tally_id"
    ) as HTMLInputElement;
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
    var title_id = document.getElementById("title_id") as HTMLSpanElement;

    function update_id(new_id: string) {
        input_tally_id.value = new_id;
        title_id.innerHTML = "(" + new_id + ")";
        document.title = `/tally (${new_id})`;
        current_id = new_id;
    }

    fetch("/api/config_state")
        .then((resp) => resp.json())
        .then((dat) => {
            update_id(dat.id);

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
        var new_id = input_tally_id.value;

        if (!current_id) {
            return
        }

        fetch(
            `/api/config?id=${current_id}&new_id=${new_id}&wifi_ssid=${wifi_ssid}&wifi_password=${wifi_password}&camera=${camera}&brightness=${brightness}`
        ).then(() => {
            alert("Done");
            update_id(new_id);
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
