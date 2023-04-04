import uos


class Creds:
    CRED_FILE = "/wifi.creds"

    def __init__(self, ssid=None, password=None, base_url=None, username=None):
        self.ssid = ssid
        self.password = password
        self.base_url = base_url
        self.username = username

    def write(self):
        """Write credentials to CRED_FILE if valid input found."""
        print(self.is_valid(), "valid")
        if self.is_valid():
            with open(self.CRED_FILE, "wb") as f:
                f.write(b",".join([self.ssid, self.password, self.base_url, self.username]))
            print("Wrote credentials to {:s}".format(self.CRED_FILE))

    def load(self):
        try:
            with open(self.CRED_FILE, "rb") as f:
                contents = f.read().split(b",")
            print("Loaded WiFi credentials from {:s}".format(self.CRED_FILE))
            if len(contents) == 4:
                self.ssid, self.password, self.base_url, self.username = contents
            if not self.is_valid():
                self.remove()
        except OSError:
            pass

        return self

    def remove(self):
        """
        1. Delete credentials file from disk.
        2. Set ssid and password to None
        """
        # print("Attempting to remove {}".format(self.CRED_FILE))
        try:
            uos.remove(self.CRED_FILE)
        except OSError:
            pass

        self.ssid = self.password = self.base_url = self.username = None

    def is_valid(self):
        # Ensure the credentials are entered as bytes
        if not isinstance(self.ssid, bytes):
            return False
        if not isinstance(self.password, bytes):
            return False
        if not isinstance(self.base_url, bytes):
            return False
        if not isinstance(self.username, bytes):
            return False

        # Ensure credentials are not None or empty
        return all((self.ssid, self.password, self.base_url, self.username))
