Import("env")

def before_build_spiffs(source, target, env):
    print("Building React App...")
    env.Execute("cd web && yarn build")

env.AddPreAction("$BUILD_DIR/spiffs.bin", before_build_spiffs)
