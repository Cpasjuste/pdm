mkdir /pdm
cp pdm /pdm/
cp -a -v data /pdm/
cp deploy/pdm /etc/init.d/
chmod a+x /etc/init.d/pdm
update-rc.d -f gpe-dm remove
rm /etc/X11/Xinit.d/99gpe-login
ln -s /etc/init.d/pdm /etc/rc0.d/K20pdm
ln -s /etc/init.d/pdm /etc/rc1.d/K20pdm
ln -s /etc/init.d/pdm /etc/rc2.d/S99pdm
ln -s /etc/init.d/pdm /etc/rc5.d/S99pdm
ln -s /etc/init.d/pdm /etc/rc6.d/K20pdm

